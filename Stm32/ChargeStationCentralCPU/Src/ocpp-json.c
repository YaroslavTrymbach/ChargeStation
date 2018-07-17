#include "ocpp-json.h"
#include "string.h"

char* buf;
int outCnt;
bool isWaitFirstElement;

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

	strcpy(buf + outCnt, parName);
	outCnt += strlen(parName);

	buf[outCnt++] = ':';
}

void addString20(char* parName, CiString20Type value){
	setParamName(parName);
	value[20] = '\0';
	strncpy(buf + outCnt, value, 20);
	outCnt += strlen(value);
}

bool jsonPackReqBootNotification(struct RequestBootNotificatation *req, char* outBuf, int* outLen){
	openJsonFormation(outBuf);

	addString20("chargePointModel", req->chargePointModel);
	addString20("chargePointVendor", req->chargePointVendor);

	closeJsonFormation();
	*outLen = outCnt;
	return true;
}
