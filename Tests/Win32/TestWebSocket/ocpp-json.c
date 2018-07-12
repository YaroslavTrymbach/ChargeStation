#include "ocpp-json.h"
#include <string.h>
#include <stdlib.h>

char* buf;
int outCnt;
bool isWaitFirstElement;

#define addIdToken addString20 

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

void setParamName(char* parName){
	checkWaitFirstElement();

	buf[outCnt++] = '"';
	strcpy(buf + outCnt, parName);
	outCnt += strlen(parName);
	buf[outCnt++] = '"';

	buf[outCnt++] = ':';
}

void addString20(char* parName, CiString20Type value){
	setParamName(parName);
	buf[outCnt++] = '"';
	value[20] = '\0';
	strncpy(buf + outCnt, value, 20);
	outCnt += strlen(value);
	buf[outCnt++] = '"';
}

void addInteger(char* parName, int value){
    char s[16];
	setParamName(parName);
	sprintf(s, "%d", value);
	//buf[outCnt++] = '"';
	strcpy(buf + outCnt, s);
	outCnt += strlen(s);
	//buf[outCnt++] = '"';*/
}

void addString(char* parName, const char* value){
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

bool jsonPackReqStatusNotification(RpcPacket *rpcPacket, RequestStatusNotification *req){
	rpcPacket->action = ACTION_STATUS_NOTIFICATION;

	openJsonFormation(rpcPacket->payload);

	addInteger("connectorId", req->connectorId);
	addString("errorCode", getChargePointErrorCodeString(req->errorCode));
	addString("status", getChargePointStatusString(req->status));

	//addIdToken("idTag", req->idTag);

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

bool isParam(const char *s, int paramName){
	const char *name;
	bool res;
	name = occpGetParamNameString(paramName);
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

bool jsonUnpackConfBootAuthorize(cJSON* json, ConfAuthorize *conf){
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

