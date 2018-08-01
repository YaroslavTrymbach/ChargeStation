#include "ocpp.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ocppConfiguration.h"

const char* EMPTY_STRING = "\0";

const char* ACTION_STR_AUTHORIZE                       = "Authorize\0";
const char* ACTION_STR_BOOT_NOTIFICATION               = "BootNotification\0";
const char* ACTION_STR_CANCEL_RESERVATION              = "CancelReservation\0";
const char* ACTION_STR_CHANGE_AVAILABILITY             = "ChangeAvailability\0";
const char* ACTION_STR_CHANGE_CONFIGURATION            = "ChangeConfiguration\0";
const char* ACTION_STR_CLEAR_CACHE                     = "ClearCache\0";
const char* ACTION_STR_CLEAR_CHARGING_PROFILE          = "ClearChargingProfile\0";
const char* ACTION_STR_DATA_TRANSFER                   = "DataTransfer\0";
const char* ACTION_STR_DIAGNOSTICS_STATUS_NOTIFICATION = "DiagnosticsStatusNotification\0";
const char* ACTION_STR_FIRMWARE_STATUS_NOTIFICATION    = "FirmwareStatusNotification\0";
const char* ACTION_STR_GET_COMPOSITE_SCHEDULE          = "GetCompositSchedule\0";
const char* ACTION_STR_GET_CONFIGURATION               = "GetConfiguration\0";
const char* ACTION_STR_GET_DIAGNOSTICS                 = "GetDiagnostics\0";
const char* ACTION_STR_GET_LOCAL_LIST_VERSION          = "GetLocalListVersion\0";
const char* ACTION_STR_HEARTBEAT                       = "Heartbeat\0";
const char* ACTION_STR_METER_VALUES                    = "MeterValues\0";
const char* ACTION_STR_REMOTE_START_TRANSACTION        = "RemoteStartTransaction\0";
const char* ACTION_STR_REMOTE_STOP_TRANSACTION         = "RemoteStopTransaction\0";
const char* ACTION_STR_RESERVE_NOW                     = "ReserveNow\0";
const char* ACTION_STR_RESET                           = "Reset\0";
const char* ACTION_STR_SEND_LOCAL_LIST                 = "SendLocalList\0";
const char* ACTION_STR_SET_CHARGING_PROFILE            = "SetChargingProfile\0";
const char* ACTION_STR_START_TRANSACTION               = "StartTransaction\0";
const char* ACTION_STR_STATUS_NOTIFICATION             = "StatusNotification\0";
const char* ACTION_STR_STOP_TRANSACTION                = "StopTransaction\0";
const char* ACTION_STR_TRIGGER_MESSAGE                 = "TriggerMessage\0";
const char* ACTION_STR_UNLOCK_CONNECTOR                = "UnlockConnector\0";
const char* ACTION_STR_UPDATE_FIRMWARE                 = "UpdateFirmware\0";

const char* OCPP_PARAM_NAME_STR_CURRENT_TIME      = "currentTime\0";
const char* OCPP_PARAM_NAME_STR_INTERVAL          = "interval\0";
const char* OCPP_PARAM_NAME_STR_STATUS            = "status\0";
const char* OCPP_PARAM_NAME_STR_ID_TAG            = "idTag\0";
const char* OCPP_PARAM_NAME_STR_ID_TAG_INFO       = "idTagInfo\0";
const char* OCPP_PARAM_NAME_STR_CONNECTOR_ID      = "connectorId\0";
const char* OCPP_PARAM_NAME_STR_METER_START       = "meterStart\0";
const char* OCPP_PARAM_NAME_STR_RESERVATION_ID    = "reservationId\0";
const char* OCPP_PARAM_NAME_STR_TIMESTAMP         = "timestamp\0";
const char* OCPP_PARAM_NAME_STR_TRANSACTION_ID    = "transactionId\0";
const char* OCPP_PARAM_NAME_STR_METER_STOP        = "meterStop";
const char* OCPP_PARAM_NAME_STR_REASON            = "reason";
const char* OCPP_PARAM_NAME_STR_KEY               = "key";
const char* OCPP_PARAM_NAME_STR_UNKNOWN_KEY       = "unknownKey";
const char* OCPP_PARAM_NAME_STR_CONFIGURATION_KEY = "configurationKey";
const char* OCPP_PARAM_NAME_STR_VALUE             = "value";
const char* OCPP_PARAM_NAME_STR_READONLY          = "readonly";
const char* OCPP_PARAM_NAME_STR_TYPE              = "type";
//const char* OCPP_PARAM_NAME_STR_ = "";


const char* AUTHORIZATION_STATUS_STR_ACCEPTED      = "Accepted\0";
const char* AUTHORIZATION_STATUS_STR_BLOCKED       = "Blocked\0";
const char* AUTHORIZATION_STATUS_STR_EXPIRED       = "Expired\0";
const char* AUTHORIZATION_STATUS_STR_INVALID       = "Invalid\0";
const char* AUTHORIZATION_STATUS_STR_CONCURRENT_TX = "ConcurrentTx\0";

const char* REGISTRATION_STATUS_STR_ACCEPTED = "Accepted\0";
const char* REGISTRATION_STATUS_STR_PENDING = "Pending\0";
const char* REGISTRATION_STATUS_STR_REJECTED = "Rejected\0";

const char* CHARGE_POINT_ERROR_CODE_STR_CONNECTOR_LOCK_FAILURE = "ConnectorLockFailure\0";
const char* CHARGE_POINT_ERROR_CODE_STR_NO_ERROR               = "NoError\0";

const char* CHARGE_POINT_STATUS_STR_AVAILABLE      = "Available\0";
const char* CHARGE_POINT_STATUS_STR_PREPARING      = "Preparing\0";
const char* CHARGE_POINT_STATUS_STR_CHARGING       = "Charging\0";
const char* CHARGE_POINT_STATUS_STR_SUSPENDED_EVSE = "SuspendedEVSE\0";
const char* CHARGE_POINT_STATUS_STR_SUSPENDED_EV   = "SuspendedEV\0";
const char* CHARGE_POINT_STATUS_STR_FINISHING      = "Finishing\0";
const char* CHARGE_POINT_STATUS_STR_RESERVED       = "Reserved\0";
const char* CHARGE_POINT_STATUS_STR_UNAVAILABLE    = "Unavailable\0";
const char* CHARGE_POINT_STATUS_STR_FAULTED        = "Faulted\0";

const char* UNLOCK_STATUS_STR_UNLOCKED      = "Unlocked\0";
const char* UNLOCK_STATUS_STR_UNLOCK_FAILED = "UnlockFailed\0";
const char* UNLOCK_STATUS_STR_NOT_SUPPORTED = "NotSupported\0";

const char* RESET_TYPE_STR_SOFT = "Soft\0";
const char* RESET_TYPE_STR_HARD = "Hard\0";

#define BOOLEAN_STR_FALSE "false\0"
#define BOOLEAN_STR_TRUE  "true\0"

#define CASE_ACTION_STR(name) case ACTION_##name: \
	                              res = ACTION_STR_##name; \
								  break

#define CASE_PARAM_STR(name) case OCPP_PARAM_##name: \
	                             res = OCPP_PARAM_NAME_STR_##name; \
			                     break

const char *getActionString(int action){
	const char* res = EMPTY_STRING;

	switch(action){		
		CASE_ACTION_STR(AUTHORIZE);
		CASE_ACTION_STR(BOOT_NOTIFICATION);
		CASE_ACTION_STR(CANCEL_RESERVATION);
		CASE_ACTION_STR(CHANGE_AVAILABILITY);
		CASE_ACTION_STR(CHANGE_CONFIGURATION);
		CASE_ACTION_STR(CLEAR_CACHE);
		CASE_ACTION_STR(CLEAR_CHARGING_PROFILE);
		CASE_ACTION_STR(DATA_TRANSFER);
		CASE_ACTION_STR(DIAGNOSTICS_STATUS_NOTIFICATION);
		CASE_ACTION_STR(FIRMWARE_STATUS_NOTIFICATION);
		CASE_ACTION_STR(GET_COMPOSITE_SCHEDULE);
		CASE_ACTION_STR(GET_CONFIGURATION);
		CASE_ACTION_STR(GET_DIAGNOSTICS);
		CASE_ACTION_STR(GET_LOCAL_LIST_VERSION);
		CASE_ACTION_STR(HEARTBEAT);
		CASE_ACTION_STR(METER_VALUES);
		CASE_ACTION_STR(REMOTE_START_TRANSACTION);
		CASE_ACTION_STR(REMOTE_STOP_TRANSACTION);
		CASE_ACTION_STR(RESERVE_NOW);
		CASE_ACTION_STR(RESET);
		CASE_ACTION_STR(SEND_LOCAL_LIST);
		CASE_ACTION_STR(SET_CHARGING_PROFILE);
		CASE_ACTION_STR(START_TRANSACTION);
		CASE_ACTION_STR(STATUS_NOTIFICATION);
		CASE_ACTION_STR(STOP_TRANSACTION);
		CASE_ACTION_STR(TRIGGER_MESSAGE);
		CASE_ACTION_STR(UNLOCK_CONNECTOR);
		CASE_ACTION_STR(UPDATE_FIRMWARE);
	}
	return res;
}

const char *getChargePointErrorCodeString(int errorCode){
	const char* res = EMPTY_STRING;

	switch(errorCode){
		case CHARGE_POINT_ERROR_CODE_CONNECTOR_LOCK_FAILURE:
			res = CHARGE_POINT_ERROR_CODE_STR_CONNECTOR_LOCK_FAILURE;
			break;
		case CHARGE_POINT_ERROR_CODE_NO_ERROR:
			res = CHARGE_POINT_ERROR_CODE_STR_NO_ERROR;
			break;
	}

	return res;
}

const char *getChargePointStatusString(int status){
	const char* res = EMPTY_STRING;

	switch(status){
		case CHARGE_POINT_STATUS_AVAILABLE:
			res = CHARGE_POINT_STATUS_STR_AVAILABLE;
			break;
		case CHARGE_POINT_STATUS_FAULTED:
			res = CHARGE_POINT_STATUS_STR_FAULTED;
			break;
	}

	return res;
}

const char *getUnlockStatusString(int status){
	const char* res = EMPTY_STRING;

	switch(status){
		case UNLOCK_STATUS_UNLOCKED:
			res = UNLOCK_STATUS_STR_UNLOCKED;
			break;
		case UNLOCK_STATUS_UNLOCK_FAILED:
			res = UNLOCK_STATUS_STR_UNLOCK_FAILED;
			break;
		case UNLOCK_STATUS_NOT_SUPPORTED:
			res = UNLOCK_STATUS_STR_NOT_SUPPORTED;
			break;
	}

	return res;
}

const char *ocppGetParamNameString(int param){
	const char* res = EMPTY_STRING;
	switch(param){
		CASE_PARAM_STR(CONNECTOR_ID);
		CASE_PARAM_STR(CURRENT_TIME);
		CASE_PARAM_STR(ID_TAG);
		CASE_PARAM_STR(ID_TAG_INFO);
		CASE_PARAM_STR(INTERVAL);
		CASE_PARAM_STR(KEY);
		CASE_PARAM_STR(METER_START);
		CASE_PARAM_STR(METER_STOP);
		CASE_PARAM_STR(REASON);
		CASE_PARAM_STR(RESERVATION_ID);
		CASE_PARAM_STR(STATUS);			
		CASE_PARAM_STR(TIMESTAMP);
		CASE_PARAM_STR(TRANSACTION_ID);
		CASE_PARAM_STR(UNKNOWN_KEY);
		CASE_PARAM_STR(CONFIGURATION_KEY);
		CASE_PARAM_STR(VALUE);
		CASE_PARAM_STR(READONLY);
		CASE_PARAM_STR(TYPE);
		//CASE_PARAM_STR();
	}
	return res;
}


int occpGetActionFromString(const char* s){

#define CHECK_ACTION(name) if(strcmp(s, ACTION_STR_##name) == 0) \
		                     return ACTION_##name;
	
	CHECK_ACTION(AUTHORIZE);
	CHECK_ACTION(BOOT_NOTIFICATION);
	CHECK_ACTION(CANCEL_RESERVATION);
	CHECK_ACTION(CHANGE_AVAILABILITY);
	CHECK_ACTION(CHANGE_CONFIGURATION);
	CHECK_ACTION(CLEAR_CACHE);
	CHECK_ACTION(CLEAR_CHARGING_PROFILE);
	CHECK_ACTION(DATA_TRANSFER);
	CHECK_ACTION(DIAGNOSTICS_STATUS_NOTIFICATION);
	CHECK_ACTION(FIRMWARE_STATUS_NOTIFICATION);
	CHECK_ACTION(GET_COMPOSITE_SCHEDULE);
	CHECK_ACTION(GET_CONFIGURATION);
	CHECK_ACTION(GET_DIAGNOSTICS);
	CHECK_ACTION(GET_LOCAL_LIST_VERSION);
	CHECK_ACTION(HEARTBEAT);
	CHECK_ACTION(METER_VALUES);
	CHECK_ACTION(REMOTE_START_TRANSACTION);
	CHECK_ACTION(REMOTE_STOP_TRANSACTION);
	CHECK_ACTION(RESERVE_NOW);
	CHECK_ACTION(RESET);
	CHECK_ACTION(SEND_LOCAL_LIST);
	CHECK_ACTION(SET_CHARGING_PROFILE);
	CHECK_ACTION(START_TRANSACTION);
	CHECK_ACTION(STATUS_NOTIFICATION);
	CHECK_ACTION(STOP_TRANSACTION);
	CHECK_ACTION(TRIGGER_MESSAGE);
	CHECK_ACTION(UNLOCK_CONNECTOR);
	CHECK_ACTION(UPDATE_FIRMWARE);
		
	return ACTION_UNKNOWN;
}

int occpGetRegistrationStatusFromString(const char* s){
	if(strcmp(s, REGISTRATION_STATUS_STR_ACCEPTED) == 0)
		return REGISTRATION_STATUS_ACCEPTED;
	if(strcmp(s, REGISTRATION_STATUS_STR_PENDING) == 0)
		return REGISTRATION_STATUS_PENDING;
	if(strcmp(s, REGISTRATION_STATUS_STR_REJECTED) == 0)
		return REGISTRATION_STATUS_REJECTED;
	return REGISTRATION_STATUS_UNKNOWN;
}

int occpGetAuthorizationStatusFromString(const char* s){
	if(strcmp(s, AUTHORIZATION_STATUS_STR_ACCEPTED) == 0)
		return AUTHORIZATION_STATUS_ACCEPTED;
	if(strcmp(s, AUTHORIZATION_STATUS_STR_BLOCKED) == 0)
		return AUTHORIZATION_STATUS_BLOCKED;
	if(strcmp(s, AUTHORIZATION_STATUS_STR_EXPIRED) == 0)
		return AUTHORIZATION_STATUS_EXPIRED;
	if(strcmp(s, AUTHORIZATION_STATUS_STR_INVALID) == 0)
		return AUTHORIZATION_STATUS_INVALID;
	if(strcmp(s, AUTHORIZATION_STATUS_STR_CONCURRENT_TX) == 0)
		return AUTHORIZATION_STATUS_CONCURRENT_TX;
	return AUTHORIZATION_STATUS_UNKNOWN;
}

int occpGetResetTypeFromString(const char* s){
	if(strcmp(s, RESET_TYPE_STR_SOFT) == 0)
		return RESET_TYPE_SOFT;
	if(strcmp(s, RESET_TYPE_STR_HARD) == 0)
		return RESET_TYPE_HARD;
	return RESET_TYPE_UNKNOWN;
}

void occpFreeCiString50TypeList(CiString50TypeListItem *list){
	CiString50TypeListItem *item;
	CiString50TypeListItem *lastItem;
	item = list;
	while(item != NULL){
		lastItem = item;
		item = lastItem->next;
		free(lastItem);
	}
}

void occpFreeKeyValueList(KeyValueListItem *list){
	KeyValueListItem *item;
	KeyValueListItem *lastItem;
	item = list;
	while(item != NULL){
		lastItem = item;
		item = lastItem->next;
		if(lastItem->data.vauleIsSet)
			free(lastItem->data.value);
		free(lastItem);
	}
}

KeyValueListItem* occpCreateKeyValueItem(int key, bool readonly){
	KeyValueListItem *item = malloc(sizeof(KeyValueListItem));
	strcpy(item->data.key, ocppGetConfigKeyString(key));
	item->data.readonly = readonly;
	item->data.vauleIsSet = false;
	item->next = NULL;
	return item;
}

KeyValueListItem* occpCreateKeyValueInt(int key, bool readonly, int value){
	KeyValueListItem *item = occpCreateKeyValueItem(key, readonly);
	item->data.value = malloc(16); //For integer value it's enough
	sprintf(item->data.value, "%d", value);
	item->data.vauleIsSet = true;
	return item;
}

KeyValueListItem* occpCreateKeyValueBool(int key, bool readonly, bool value){
	KeyValueListItem *item = occpCreateKeyValueItem(key, readonly);
	item->data.value = malloc(8); //For bool value it's enough
	strcpy(item->data.value, value ? BOOLEAN_STR_TRUE : BOOLEAN_STR_FALSE);
	item->data.vauleIsSet = true;
	return item;
}
