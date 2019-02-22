#include "ocpp-json.h"
#include <string.h>
#include <stdlib.h>

char* buf;
int outCnt;
bool isWaitFirstElement;

#define addIdToken addString20 

#define paramStr(param) ocppGetParamNameString(OCPP_PARAM_##param)

const char* BOOLEAN_STR_FALSE = "false\0";
const char* BOOLEAN_STR_TRUE  = "true\0";

void addString(const char* parName, const char* value);


void openJsonFormation(char* outBuf){
	buf = outBuf;
	outCnt = 0;
	isWaitFirstElement = true;

	buf[outCnt++] = '{';
}

void closeJsonFormation(){
	buf[outCnt++] = '}';
}

void checkWaitFirstElement(){
	if(!isWaitFirstElement){
		buf[outCnt++] = ',';
	}
	else{
		isWaitFirstElement = false;
	}
}

void setParamName(const char* parName){
	checkWaitFirstElement();

	buf[outCnt++] = '"';
	strcpy(buf + outCnt, parName);
	outCnt += strlen(parName);
	buf[outCnt++] = '"';

	buf[outCnt++] = ':';
}

void addValueString(char* value, int max_cnt){
	buf[outCnt++] = '"';
	if(max_cnt > 0){
		value[max_cnt] = '\0';
		strncpy(buf + outCnt, value, max_cnt);
	}
	else{
		strcpy(buf + outCnt, value);
	}
	outCnt += strlen(value);
	buf[outCnt++] = '"';
}

#define addValueString20(value) addValueString(value, 20)
#define addValueString50(value) addValueString(value, 50)

void addBoolean(const char* parName, bool value){
	const char *valueStr;
	setParamName(parName);
	valueStr = (value) ? BOOLEAN_STR_TRUE : BOOLEAN_STR_FALSE;
	strcpy(buf + outCnt, valueStr);
	outCnt += strlen(valueStr);
}

void addString20(const char* parName, CiString20Type value){
	/*
	setParamName(parName);
	buf[outCnt++] = '"';
	value[20] = '\0';
	strncpy(buf + outCnt, value, 20);
	outCnt += strlen(value);
	buf[outCnt++] = '"';*/
	setParamName(parName);
	addValueString20(value);
}

void addString50(const char* parName, CiString50Type value){
	setParamName(parName);
	addValueString50(value);
}

void addInteger(const char* parName, int value){
    char s[16];
	setParamName(parName);
	sprintf(s, "%d", value);
	strcpy(buf + outCnt, s);
	outCnt += strlen(s);
}

void addDateTime(const char* parName, dateTime value){
	char s[32];
	setParamName(parName);
	buf[outCnt++] = '"';

	// 2017-08-22T06:11:00.000Z
	sprintf(s, "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d.000Z", 
		value.tm_year, value.tm_mon, value.tm_mday, value.tm_hour, value.tm_min, value.tm_sec);
	strcpy(buf + outCnt, s);
	outCnt += strlen(s);

	buf[outCnt++] = '"';
}

void addString50list(const char* parName, CiString50TypeListItem *list){
	CiString50TypeListItem* item;
	bool firstElement = true;
	setParamName(parName);
	buf[outCnt++] = '[';

	item = list;
	while(item != NULL){
		if(firstElement)
			firstElement = false;
		else
			buf[outCnt++] = ',';
		addValueString50(item->data);
		item = item->next;
	}

	//strcpy(buf + outCnt, value);
	//outCnt += strlen(value);
	buf[outCnt++] = ']';
}

void addKeyValue(KeyValueListItem *item){
	isWaitFirstElement = true;
	buf[outCnt++] = '{';
	addString50(paramStr(KEY), item->data.key);
	addBoolean(paramStr(READONLY), item->data.readonly);
	if(item->data.vauleIsSet){
		addString(paramStr(VALUE), item->data.value);
	}
	buf[outCnt++] = '}';
}

void addKeyValueList(const char* parName, KeyValueListItem *list){
	KeyValueListItem* item;
	bool firstElement = true;
	setParamName(parName);
	buf[outCnt++] = '[';

	item = list;
	while(item != NULL){
		if(firstElement)
			firstElement = false;
		else
			buf[outCnt++] = ',';
		addKeyValue(item);
		//addValueString50(item->data);
		item = item->next;
	}

	buf[outCnt++] = ']';
}

void addSampledValue(SampledValueListItem *item){
	isWaitFirstElement = true;
	buf[outCnt++] = '{';
	addInteger(paramStr(VALUE), item->value);
	buf[outCnt++] = '}';
}

void addSampledValueList(const char* parName, SampledValueListItem *list){
	SampledValueListItem* item;
	bool firstElement = true;
	setParamName(parName);
	buf[outCnt++] = '[';

	item = list;
	while(item != NULL){
		if(firstElement)
			firstElement = false;
		else
			buf[outCnt++] = ',';
		addSampledValue(item);
		item = item->next;
	}

	buf[outCnt++] = ']';
}

void addMeterValue(MeterValueListItem *item){
	isWaitFirstElement = true;
	buf[outCnt++] = '{';
	addDateTime(paramStr(TIMESTAMP), item->meterValue.timestamp);
	addSampledValueList(paramStr(SAMPLED_VALUE), item->meterValue.samledValue);
	buf[outCnt++] = '}';
}

void addMeterValueList(const char* parName, MeterValueListItem *list){
	MeterValueListItem* item;
	bool firstElement = true;
	setParamName(parName);
	buf[outCnt++] = '[';

	item = list;
	while(item != NULL){
		if(firstElement)
			firstElement = false;
		else
			buf[outCnt++] = ',';
		addMeterValue(item);
		item = item->next;
	}

	buf[outCnt++] = ']';
}

void addString(const char* parName, const char* value){
	setParamName(parName);
	buf[outCnt++] = '"';
	strcpy(buf + outCnt, value);
	outCnt += strlen(value);
	buf[outCnt++] = '"';
}

void fillDateTimeFromString(dateTime *dt, const char* str){
	char s[8];
	int val;
	// 2017-08-22T06:11:00.000Z
	dt->tm_mday = 1;
	dt->tm_mon = 1;
	dt->tm_year = 2000;
	dt->tm_hour = 0;
	dt->tm_min = 0;
	dt->tm_sec = 0;

	if(strlen(str) < 20)
		return;
	if((str[4] != '-') || (str[7] != '-') || (str[10] != 'T'))
		return;

	//Year
	strncpy(s, str, 4);
	s[4] = '\0';
	val = atoi(s);
	if(val != 0)
		dt->tm_year = val;

	s[2] = '\0';

	//Month
	strncpy(s, str + 5, 2);
	val = atoi(s);
	if(val != 0)
		dt->tm_mon = val;

	//Day
	strncpy(s, str + 8, 2);
	val = atoi(s);
	if(val != 0)
		dt->tm_mday = val;

	//Hour
	strncpy(s, str + 11, 2);
	dt->tm_hour = atoi(s);

	//Minutes
	strncpy(s, str + 14, 2);
	dt->tm_min = atoi(s);

	//Seconds
	strncpy(s, str + 17, 2);
	dt->tm_sec = atoi(s);
}

bool jsonPackReqBootNotification(RpcPacket *rpcPacket, RequestBootNotification *req){
	rpcPacket->action = ACTION_BOOT_NOTIFICATION;

	openJsonFormation(rpcPacket->payload);

	addString20("chargePointModel", req->chargePointModel);
	addString20("chargePointVendor", req->chargePointVendor);

	closeJsonFormation();
	rpcPacket->payloadLen = outCnt;
	return true;
}

bool jsonPackReqHeartbeat(RpcPacket *rpcPacket){
		rpcPacket->action = ACTION_HEARTBEAT;

	openJsonFormation(rpcPacket->payload);

	//No field are defined

	closeJsonFormation();
	rpcPacket->payloadLen = outCnt;
	return true;
}

bool jsonPackReqMeterValues(RpcPacket *rpcPacket, RequestMeterValues *req){
	rpcPacket->action = ACTION_METER_VALUES;

	openJsonFormation(rpcPacket->payload);
	addInteger(ocppGetParamNameString(OCPP_PARAM_CONNECTOR_ID), req->connectorId);
	if(req->useTransactionId){
		addInteger(ocppGetParamNameString(OCPP_PARAM_TRANSACTION_ID), req->transactionId);
	}
	addMeterValueList(ocppGetParamNameString(OCPP_PARAM_METER_VALUE), req->meterValue);


	closeJsonFormation();
	rpcPacket->payloadLen = outCnt;
	return true;
}

bool jsonPackReqStartTransaction(RpcPacket *rpcPacket, RequestStartTransaction *req){
	rpcPacket->action = ACTION_START_TRANSACTION;

	openJsonFormation(rpcPacket->payload);
	addInteger(ocppGetParamNameString(OCPP_PARAM_CONNECTOR_ID), req->connectorId);
	addIdToken(ocppGetParamNameString(OCPP_PARAM_ID_TAG), req->idTag);
	addInteger(ocppGetParamNameString(OCPP_PARAM_METER_START), req->meterStart);
	addDateTime(paramStr(TIMESTAMP), req->timestamp);

	if(req->useReservationId)
		addInteger(ocppGetParamNameString(OCPP_PARAM_RESERVATION_ID), req->reservationId);

	closeJsonFormation();
	rpcPacket->payloadLen = outCnt;
	return true;
}

bool jsonPackReqStopTransaction(RpcPacket *rpcPacket, RequestStopTransaction *req){
	rpcPacket->action = ACTION_STOP_TRANSACTION;

	openJsonFormation(rpcPacket->payload);
	if(req->useIdTag)
		addIdToken(paramStr(ID_TAG), req->idTag);
	addInteger(paramStr(METER_STOP), req->meterStop);
	addDateTime(paramStr(TIMESTAMP), req->timestamp);
	addInteger(paramStr(TRANSACTION_ID), req->transactionId);

	if(req->useReason)
		addInteger(paramStr(REASON), req->reason);

	closeJsonFormation();
	rpcPacket->payloadLen = outCnt;
	return true;
}

bool jsonPackReqStatusNotification(RpcPacket *rpcPacket, RequestStatusNotification *req){
	rpcPacket->action = ACTION_STATUS_NOTIFICATION;

	openJsonFormation(rpcPacket->payload);

	addInteger("connectorId", req->connectorId);
	addString("errorCode", getChargePointErrorCodeString(req->errorCode));
	addString("status", getChargePointStatusString(req->status));

	closeJsonFormation();
	rpcPacket->payloadLen = outCnt;
	return true;
}


bool jsonPackReqAuthorize(RpcPacket *rpcPacket, RequestAuthorize *req){
	rpcPacket->action = ACTION_AUTHORIZE;

	openJsonFormation(rpcPacket->payload);

	addIdToken("idTag", req->idTag);

	closeJsonFormation();
	rpcPacket->payloadLen = outCnt;
	return true;
}

bool jsonPackConfUnlockConnector(RpcPacket *rpcPacket, ConfUnlockConnector *conf){
	openJsonFormation(rpcPacket->payload);

	addString("status", getUnlockStatusString(conf->status));

	closeJsonFormation();
	rpcPacket->payloadLen = outCnt;
	return true;
}

bool jsonPackConfChangeConfiguration(RpcPacket *rpcPacket, ConfChangeConfiguration *conf){
	openJsonFormation(rpcPacket->payload);

	addString(paramStr(STATUS), getConfigurationStatusString(conf->status));

	closeJsonFormation();
	rpcPacket->payloadLen = outCnt;
	return true;
}

bool jsonPackConfGetConfiguration(RpcPacket *rpcPacket, ConfGetConfiguration *conf){
	openJsonFormation(rpcPacket->payload);

	//addString("status", getUnlockStatusString(conf->status));
	addString50list(paramStr(UNKNOWN_KEY), conf->unknownKey);
	addKeyValueList(paramStr(CONFIGURATION_KEY), conf->configurationKey);

	closeJsonFormation();
	rpcPacket->payloadLen = outCnt;
	return true;
}

bool jsonPackConfGetLocalListVersion(RpcPacket *rpcPacket, ConfGetLocalListVersion *conf){
	openJsonFormation(rpcPacket->payload);

	addInteger(paramStr(LIST_VERSION), conf->listVersion);

	closeJsonFormation();
	rpcPacket->payloadLen = outCnt;
	return true;
}

bool isParam(const char *s, int paramName){
	const char *name;
	bool res;
	name = ocppGetParamNameString(paramName);
	res = (strcmp(s, name) == 0);
	return res;
}

bool jsonUnpackConfBootNotification(cJSON* json, ConfBootNotifiaction *conf){
    cJSON* jsonElement;
	jsonElement = json->child;

	while(jsonElement != NULL){
		if(jsonElement->type == cJSON_String){
			if(isParam(jsonElement->string, OCPP_PARAM_CURRENT_TIME)){
				fillDateTimeFromString(&conf->currentTime, jsonElement->valuestring);
			}
			else if(isParam(jsonElement->string, OCPP_PARAM_STATUS)){
				conf->status = occpGetRegistrationStatusFromString(jsonElement->valuestring);
			}
		}
		else if(jsonElement->type == cJSON_Number){
			if(isParam(jsonElement->string, OCPP_PARAM_INTERVAL)){
				conf->interval = jsonElement->valueint;
			}
		}
		jsonElement = jsonElement->next;
	}
	return true;
}

bool jsonUnpackConfHeartbeat(cJSON* json, ConfHeartbeat *conf){
	cJSON* jsonElement;
	jsonElement = json->child;

	while(jsonElement != NULL){
		if(jsonElement->type == cJSON_String){
			if(isParam(jsonElement->string, OCPP_PARAM_CURRENT_TIME)){
				fillDateTimeFromString(&conf->currentTime, jsonElement->valuestring);
			}
		}
		jsonElement = jsonElement->next;
	}
	return true;
}

bool jsonUnpackParamIdTagInfo(cJSON* json, IdTagInfo *param){
	cJSON* jsonElement;
	jsonElement = json->child;

	while(jsonElement != NULL){

		if(jsonElement->type == cJSON_String){
			if(isParam(jsonElement->string, OCPP_PARAM_STATUS)){
				param->status = occpGetAuthorizationStatusFromString(jsonElement->valuestring);
			}
		}

		jsonElement = jsonElement->next;
	}
	return true;
}

bool jsonUnpackConfAuthorize(cJSON* json, ConfAuthorize *conf){
	cJSON* jsonElement;
	jsonElement = json->child;

	while(jsonElement != NULL){
		
		if(jsonElement->type == cJSON_Object){
			if(isParam(jsonElement->string, OCPP_PARAM_ID_TAG_INFO)){
				jsonUnpackParamIdTagInfo(jsonElement, &(conf->idTagInfo));
			}
		}
		jsonElement = jsonElement->next;
	}
	return true;
}

bool jsonUnpackConfStartTransaction(cJSON* json, ConfStartTransaction *conf){
	cJSON* jsonElement;
	jsonElement = json->child;

	while(jsonElement != NULL){
		
		if(jsonElement->type == cJSON_Object){
			if(isParam(jsonElement->string, OCPP_PARAM_ID_TAG_INFO)){
				jsonUnpackParamIdTagInfo(jsonElement, &(conf->idTagInfo));
			}
		}
		else if(jsonElement->type == cJSON_Number){
			if(isParam(jsonElement->string, OCPP_PARAM_TRANSACTION_ID)){
				conf->transactionId = jsonElement->valueint;
			}
		}
		jsonElement = jsonElement->next;
	}
	return true;
}

bool jsonUnpackReqUnlockConnector(cJSON* json, RequestUnlockConnector *req){
	cJSON* jsonElement;
	jsonElement = json->child;

	while(jsonElement != NULL){
		
		if(jsonElement->type == cJSON_Number){
			if(isParam(jsonElement->string, OCPP_PARAM_CONNECTOR_ID)){
				req->connectorId = jsonElement->valueint;
			}
		}
		jsonElement = jsonElement->next;
	}
	return true;
}

bool jsonUnpackReqChangeConfiguration(cJSON* json, RequestChangeConfiguration *req){
	int size;
	cJSON* jsonElement;
	jsonElement = json->child;

	req->value = NULL;
	while(jsonElement != NULL){
		
		if(jsonElement->type == cJSON_String){
			if(isParam(jsonElement->string, OCPP_PARAM_KEY)){
				strcpy(req->key, jsonElement->valuestring);
			}
			else if(isParam(jsonElement->string, OCPP_PARAM_VALUE)){
				size = strlen(jsonElement->valuestring) + 1;
				req->value = malloc(size);
				strcpy(req->value, jsonElement->valuestring);
			}
		}

		jsonElement = jsonElement->next;
	}
	return true;
}

bool jsonUnpackReqGetConfiguration(cJSON* json, RequestGetConfiguration *req){
	cJSON* jsonElement;
	cJSON* jsonIter;
	jsonElement = json->child;
	req->keySize = 0;

	while(jsonElement != NULL){
		
		if(jsonElement->type == cJSON_Array){
			if(isParam(jsonElement->string, OCPP_PARAM_KEY)){
				jsonIter = jsonElement->child;
				while(jsonIter != NULL){
					strcpy(req->key[req->keySize++], jsonIter->valuestring);
					if(req->keySize >= CONFIGURATION_GET_MAX_KEYS)
						break;
					jsonIter = jsonIter->next;
				}
			}
		}
		jsonElement = jsonElement->next;
	}
	return true;
}

bool jsonUnpackReqReset(cJSON* json, RequestReset *req){
	cJSON* jsonElement;
	jsonElement = json->child;

	while(jsonElement != NULL){
		
		if(jsonElement->type == cJSON_String){
			if(isParam(jsonElement->string, OCPP_PARAM_TYPE)){
				req->type = occpGetResetTypeFromString(jsonElement->valuestring);
			}
		}
		jsonElement = jsonElement->next;
	}

	return true;
}


