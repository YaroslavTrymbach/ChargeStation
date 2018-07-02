#include "rpc.h"
#include "rng.h"
#include "ocpp.h"
#include <string.h>

#define PARAM_NONE      0
#define PARAM_UNIQUE_ID 1
#define PARAM_ACTION    2
#define PARAM_PAYLOAD   10

#define PARSE_STATE_STRING_START  1
#define PARSE_STATE_STRING_FINISH 2
#define PARSE_STATE_DELIMITER     3

int parseCurParam;
uint8_t parseMesType;
char parsedStr[256];
int parsedStrCnt;
int maxParsedStrSize;
bool maxParsedStrError; //Flag that mean of needs to breaks parsing when overhead maxParsedStrSize

bool fixParsedParam(RpcPacket *outPacket){
	parsedStr[parsedStrCnt] = '\0';
	switch(parseCurParam){
		case PARAM_UNIQUE_ID:
			strcpy(outPacket->uniqueId, parsedStr);  
			break;
		case PARAM_ACTION:
			outPacket->action = occpGetActionFromString(parsedStr);
			break;
	}
	return true;
}

void setParsedParam(int param){
	parseCurParam = param;
	parsedStrCnt = 0;

	switch(parseCurParam){
		case PARAM_UNIQUE_ID:
			maxParsedStrSize = RPC_GUID_MAX_LENGTH;
			maxParsedStrError = true;
			break;
		case PARAM_ACTION:
			maxParsedStrSize = 128;
			maxParsedStrError = false;	
			break;
	}
}

void setNextParsedParam(uint8_t mesType){
	switch(mesType){
		case MES_TYPE_CALL:
			switch(parseCurParam){
				case PARAM_UNIQUE_ID:
					setParsedParam(PARAM_ACTION);
					break;
				case PARAM_ACTION:
					setParsedParam(PARAM_PAYLOAD);
					break;
				default:
					setParsedParam(PARAM_NONE);
			}
			break;
		case MES_TYPE_CALLRESULT:
			switch(parseCurParam){
				case PARAM_UNIQUE_ID:
					setParsedParam(PARAM_PAYLOAD);
					break;
				default:
					setParsedParam(PARAM_NONE);
			}
			break;
		case MES_TYPE_CALLERROR:
			break;
		default:
			setParsedParam(PARAM_NONE);
	}
}


bool fillRpcCallData(RpcPacket *outPacket, char* outData, int* outLen){
	int cnt = 0;
	//char GUID[RPC_GUID_MAX_LENGTH];
	const char* actionStr = getActionString(outPacket->action);

	if(outPacket->payloadLen > outPacket->payloadSize){
		*outLen = 0;
		return false;
	}

	outData[cnt++] = '[';

	//MessageTypeId
	outData[cnt++] = MES_TYPE_CALL;
	strcpy(outData + cnt, ",\"");
	cnt += 2;
	//GUID
	generateGUID(outPacket->uniqueId);
	strncpy(outData + cnt, outPacket->uniqueId, RPC_GUID_MAX_LENGTH);
	cnt += 36;
	strcpy(outData + cnt, "\",\"");
	cnt += 3;
	//Action
	strcpy(outData + cnt, actionStr);
	cnt += strlen(actionStr);
	strcpy(outData + cnt, "\",");
	cnt += 2;
	//Payload
	memcpy(outData + cnt, outPacket->payload, outPacket->payloadLen);
	cnt += outPacket->payloadLen;
	outData[cnt++] = ']';

	*outLen = cnt;
	return true;
}

bool fillRpcCallResultData(RpcPacket *outPacket, char* outData, int* outLen){
	int cnt = 0;
	const char* actionStr = getActionString(outPacket->action);

	if(outPacket->payloadLen > outPacket->payloadSize){
		*outLen = 0;
		return false;
	}

	outData[cnt++] = '[';

	//MessageTypeId
	outData[cnt++] = MES_TYPE_CALLRESULT;
	strcpy(outData + cnt, ",\"");
	cnt += 2;
	//GUID
	strncpy(outData + cnt, outPacket->uniqueId, RPC_GUID_MAX_LENGTH);
	cnt += 36;
	strcpy(outData + cnt, "\",");
	cnt += 2;
	//Payload
	memcpy(outData + cnt, outPacket->payload, outPacket->payloadLen);
	cnt += outPacket->payloadLen;
	outData[cnt++] = ']';

	*outLen = cnt;
	return true;
}



bool parseRpcInputData(char* data, int dataLen, RpcPacket *outPacket){
	char *p;
	char *lastChar;
	char *paramStr;
	int state, len;
	bool needBrake;
	
	if(dataLen < 9)
		return false;
	lastChar = data + (dataLen - 1);
	if((data[0] != '[') || (*lastChar != ']'))
		return false;

	p = data + 1;
	switch(*p){
		case MES_TYPE_CALL:
		case MES_TYPE_CALLRESULT:
		case MES_TYPE_CALLERROR:
			parseMesType = *(p++);
			outPacket->messageType = parseMesType;
			break;
		default:
			return false;
	}

	if(*p != ',')
		return false;

	setParsedParam(PARAM_UNIQUE_ID);
	state = PARSE_STATE_STRING_START;

	needBrake = false;
	while(++p < lastChar){
		if(needBrake)
			break;

		switch(state){
			case PARSE_STATE_STRING_START:
				if(*p != '"')
					return false;
				state = PARSE_STATE_STRING_FINISH;
				break;
			case PARSE_STATE_STRING_FINISH:
				if(*p == '"'){
					if(!fixParsedParam(outPacket))
						return false;
					setNextParsedParam(parseMesType);
					state = PARSE_STATE_DELIMITER;
				}
				else{
					if(parsedStrCnt < maxParsedStrSize){
						parsedStr[parsedStrCnt++] = *p;
					}
					else if(maxParsedStrError){
						return false;
					}
				}
				break;
			case PARSE_STATE_DELIMITER:
				if(*p != ',')
					return false;
				if((parseCurParam == PARAM_PAYLOAD) || (parseCurParam == PARAM_NONE))
					needBrake = true;
				else
					state = PARSE_STATE_STRING_START;
				break;
		}
	}

	if(parseCurParam != PARAM_PAYLOAD)
		return false;

	//Left only payload
	len = lastChar - p;
	if(len > (outPacket->payloadSize - 1))
		len = (outPacket->payloadSize - 1);
	outPacket->payloadLen = len;
	if(outPacket->payload == 0)
		return false;
	memcpy(outPacket->payload, p, len);
	outPacket->payload[len] = '\0';

	return true;
}
