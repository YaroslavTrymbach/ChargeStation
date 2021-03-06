#include "webSocket.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "base64.h"
#include "rng.h"

#define HEADER_HOST                "Host"
#define HEADER_UPGRADE             "Upgrade"
#define HEADER_CONNECTION          "Connection"
#define HEADER_WEB_SOCKET_KEY      "Sec-WebSocket-Key"
#define HEADER_WEB_SOCKET_PROTOCOL "Sec-WebSocket-Protocol"
#define HEADER_WEB_SOCKET_VERSION  "Sec-WebSocket-Version"

#define EMPTY_LINE "\r\n"

#define WS_HEAD_MASK_FIN         0x80
#define WS_HEAD_MASKED           0x80

#define SERVER_HOST_SIZE 64
#define MAX_PAYLOAD_SIZE 512

#define INPUT_STATE_START             0
#define INPUT_STATE_PAYLOAD_LENGTH    1
#define INPUT_STATE_PAYLOAD_LENGTH_EX 2
#define INPUT_STATE_PAYLOAD           4

typedef struct _LineSearch{
	char* Line;
	char* Next;
}LineSearch;

char head_str[64];
char *req_ptr;
int req_size;

/*
int server_port = 80;
char server_host[SERVER_HOST_SIZE];*/

uint8_t payload[MAX_PAYLOAD_SIZE];
int payloadLengthBit = 32;
int payloadGetCnt = 0;
int payloadWaitedSize = 0;
int inputState = INPUT_STATE_START;

bool WebSocket_GetInputPayloadData(uint8_t *buf, int bufSize, int *outSize){
	*outSize = payloadWaitedSize;
	if(bufSize < *outSize)
		return false;

	memcpy(buf, payload, payloadWaitedSize);	

	return true;
}

void WebSocket_ClearInBuffer(){
	payloadGetCnt = 0;
	inputState = INPUT_STATE_START;
}

bool WebSocket_ProcessInputData(uint8_t *data, int dataLen, WebSocketInputDataState *state){
	int inCnt = 0;
	int inLeft, waitedLeft;
	uint8_t d;
	int chunkSize;
	state->status = WS_PROCESS_STATUS_WAITING;
	while(inCnt < dataLen){
		switch(inputState){
			case INPUT_STATE_START:
				d = data[inCnt++];
				state->opCode = (d & 0x0F);
				inputState = INPUT_STATE_PAYLOAD_LENGTH;
				break;
			case INPUT_STATE_PAYLOAD_LENGTH:
				d = (data[inCnt++] & 0x7F);
				payloadWaitedSize = 0;
				if(d == 127){
					inputState = INPUT_STATE_PAYLOAD_LENGTH_EX;
					payloadLengthBit = 32;
				}
				else if(d == 126){
					inputState = INPUT_STATE_PAYLOAD_LENGTH_EX;
					payloadLengthBit = 16;
				}
				else{
					payloadWaitedSize = d;
					if(payloadWaitedSize > 0)
						inputState = INPUT_STATE_PAYLOAD;
					else{
						//No data
						state->status = WS_PROCESS_STATUS_FINISHED;
						return true;
					}

				}
				break;
			case INPUT_STATE_PAYLOAD_LENGTH_EX:
				d = data[inCnt++];
				payloadLengthBit -= 8;
				payloadWaitedSize |= ((int) d) << payloadLengthBit;
				if(payloadLengthBit <= 0){
					inputState = INPUT_STATE_PAYLOAD;
				}
				break;
			case INPUT_STATE_PAYLOAD:
				inLeft = dataLen - inCnt;
				waitedLeft = payloadWaitedSize - payloadGetCnt;
				chunkSize = (waitedLeft <= inLeft) ? waitedLeft : inLeft;
				memcpy(payload + payloadGetCnt, data + inCnt, chunkSize);
				inCnt += chunkSize;
				payloadGetCnt += chunkSize;
				if(payloadGetCnt >= payloadWaitedSize){
					state->status = WS_PROCESS_STATUS_FINISHED;
					return true;
				}
				break;
			default:
				return false;
		}
	}
	return true;
}

bool WebSocket_GetInputPayload(uint8_t *data, int *outLen){
	return true;
}

void addStartLine(char *uri){
	int len;
	sprintf(head_str, "GET %s HTTP/1.1\r\n", uri);
	len = strlen(head_str);
	strncpy(req_ptr, head_str, len);
	req_size += len;
	req_ptr += len;
}


void addRequestEmptyLine(){
	strcpy(req_ptr, EMPTY_LINE);
	req_size += 2;
	req_ptr += 2;
}

void addHeader(char *header, char *value){ 
	int len;
	sprintf(head_str, "%s: %s\r\n", header, value);
	len = strlen(head_str);
	strncpy(req_ptr, head_str, len);
	req_size += len;
	req_ptr += len;
}

int fillHandshakeRequest(WebSocketConnectionParams *params, char *buf){
	char valueStr[128];
	char *base64str;
	int base64len;
	req_ptr = buf;
	req_size = 0;
	addStartLine(params->uri);
	sprintf(valueStr, "%s:%d", params->server_host, params->server_port);
	addHeader(HEADER_HOST, valueStr);
	addHeader(HEADER_UPGRADE, "websocket");
	addHeader(HEADER_CONNECTION, "Upgrade");
	strcpy(valueStr, "0123456789ABCDEF");
	base64str = base64_encode(valueStr, 16, &base64len);
	strncpy(valueStr, base64str, base64len);
	valueStr[base64len] = '\0';
	addHeader(HEADER_WEB_SOCKET_KEY, valueStr);
	free(base64str);
	addHeader(HEADER_WEB_SOCKET_PROTOCOL, "ocpp1.6");
	addHeader(HEADER_WEB_SOCKET_VERSION, "13");
	addRequestEmptyLine();
	return req_size;
}

void applyMask(char* buf, int len, uint32_t mask){
	int i, blockCnt;
	uint32_t *bkPtr = (uint32_t*)buf;
	blockCnt = (len / 4) + (((len % 4) == 0) ? 0 : 1);
	for(i = 0; i < blockCnt; i++){
		*bkPtr ^= mask;
		bkPtr += 1;
	}
}

WebSocketStatus fillWebSocketPongData(char* inData, int inLen, char* outData, int *outLen){
	int cnt = 0;
	uint32_t mask;
	bool useMask = true;
	outData[cnt++] = WS_HEAD_MASK_FIN | WEB_SOCKET_OPCODE_PONG;
	if(inLen < 126){
		outData[cnt++] = (inLen & 0x7F);
	}
	else{
		outData[cnt++] = 0x7E;
		outData[cnt++] = (inLen >> 8) & 0xFF;
		outData[cnt++] = inLen & 0xFF;
	}

	if(useMask){
		outData[1] |= WS_HEAD_MASKED;
		mask = generateRnd32();

		memcpy(outData + cnt, &mask, 4);
		cnt += 4;

		applyMask(inData, inLen, mask);
	}

	memcpy(outData + cnt, inData, inLen);
	cnt += inLen;

	*outLen = cnt;
	return WS_OK;
}

WebSocketStatus fillWebSocketClientSendData(char* inData, int inLen, char* outData, int *outLen){
	int cnt = 0;
	uint32_t mask;
	outData[cnt++] = WS_HEAD_MASK_FIN | WEB_SOCKET_OPCODE_TEXT;
	if(inLen < 126){
		outData[cnt++] = WS_HEAD_MASKED | (inLen & 0x7F);
	}
	else{
		outData[cnt++] = WS_HEAD_MASKED | 0x7E;
		//memcpy(outData + cnt, &inLen, 2);
		//cnt += 2;

		outData[cnt++] = (inLen >> 8) & 0xFF;
		outData[cnt++] = inLen & 0xFF;

		//outData[cnt++] = 0x00;
		//outData[cnt++] = 0x00;
	}

	//mask = 0x12345678;
	//mask = 0x19191919;
	mask = generateRnd32();

	memcpy(outData + cnt, &mask, 4);
	cnt += 4;

	applyMask(inData, inLen, mask);

	memcpy(outData + cnt, inData, inLen);
	cnt += inLen;

	*outLen = cnt;
	return WS_OK;
}

bool getNextLine(char **data, char *line){
	char *p;
	int len;
	p = strstr(*data, "\r\n");
	if(p == NULL)
		return false;
	len = p - *data;
	strncpy(line, *data, len);
	line[len] = '\0';
	*data = p + 2;
	return true;
}

bool parseStatusLine(char* data, WebSocketHttpHeader* outHeader){
	char *p1, *p2;
	char codeStr[4];
	int code;
	p1 = strchr(data, ' ');
	if(p1 == NULL)
		return false;
	p1 += 1;
	p2 = strchr(p1, ' ');
	if(p2 == NULL)
		return false;
	if((p2 - p1) != 3)
		return false;
	strncpy(codeStr, p1, 3);
	codeStr[3] = '\0';
	code = atoi(codeStr);
	if(code != 0)
		outHeader->StatusCode = code;
	return true;
}

bool WebSocket_parseHttpAnswerHeader(char* data, WebSocketHttpHeader* outHeader){
	char line[256];
	char **p;
	outHeader->StatusCode = 0;
	p = &data;
	if(!getNextLine(p, line))
		return false;

	if(!parseStatusLine(line, outHeader))
		return false;

	return true;
}

void foo(void){
  printf("foo\n");
}