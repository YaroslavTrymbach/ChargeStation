#include "indication.h"

#include <string.h>
#include "display.h"
#include "ocpp.h"

bool isMessageShown = false;
int messageShownEndTime = 0;

static ChargePointConnector *connector = NULL;

void Indication_Init(ChargePointConnector *conn){
	connector = conn;
}

void Indication_PrintAllChannels(void){
	int i;
	for(i = 0; i < CONFIGURATION_NUMBER_OF_CONNECTORS; i++){
		Indication_PrintChannel(i);
	}
};

void Indication_PrintChannel(int channel){
	char s[20];
	char endS[20];
	int endPos, cnt, i;
	ChargePointConnector *conn;
	bool showMeter = false;
	
	if(isMessageShown)
		return;
	
	conn = &connector[channel];
	sprintf(s, "C%d ", channel + 1);
	
	switch(conn->status){
		case CHARGE_POINT_STATUS_AVAILABLE:
			strcat(s, "Free");
			break;
		case CHARGE_POINT_STATUS_PREPARING:
			strcat(s, "Ready");
			break;
		case CHARGE_POINT_STATUS_CHARGING:			
	    strcat(s, "Charging");	
		  showMeter = true;
			break;
		case CHARGE_POINT_STATUS_SUSPENDED_EVSE:
			strcat(s, "SuspEVSE");
		  showMeter = true;
			break;
		case CHARGE_POINT_STATUS_SUSPENDED_EV:
			strcat(s, "SuspEV");
		  showMeter = true;
			break;
		case CHARGE_POINT_STATUS_FINISHING:
			strcat(s, "Finishing");
		  showMeter = true;
			break;
		case CHARGE_POINT_STATUS_RESERVED:
			strcat(s, "Reserved");
			break;
		case CHARGE_POINT_STATUS_UNAVAILABLE:
			strcat(s, "Unavailable");
			break;
		case CHARGE_POINT_STATUS_FAULTED:
			sprintf(endS, "Error  Code %.2d", conn->errorCode);
			strcat(s, endS);
			break;
	}
	
	if(showMeter){
		sprintf(endS, "%.5d", conn->meterValue - conn->chargeTransaction.startMeterValue);
		endPos = Display_getWidth() - strlen(endS);
		cnt = endPos - strlen(s);
		for(i = 0; i < cnt; i++){
			strcat(s, " ");
		}
		strcat(s, endS);
	}
	
	Display_PrintStrLeft(channel, s);
};

void Indication_ShowMessage(char *s1, char *s2, int time){
	Display_clear();
	if(s1 != NULL)
		Display_PrintStrCenter(1, s1);
	if(s2 != NULL)
		Display_PrintStrCenter(2, s2);
	messageShownEndTime = HAL_GetTick() + time;
	isMessageShown = true;
}

void Indication_CheckMessage(void){
	if(!isMessageShown)
		return;
	
	if(HAL_GetTick() >= messageShownEndTime){
		//Hide messages
		isMessageShown = false;
		Indication_PrintAllChannels();
	}
}
