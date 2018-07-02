#include "ocpp.h"
#include "string.h"

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
const char* ACTION_STR_GET_DIAGNOSTICS                 = "\0";
const char* ACTION_STR_GET_LOCAL_LIST_VERSION          = "\0";
const char* ACTION_STR_HEARTBEAT                       = "\0";
const char* ACTION_STR_METER_VALUES                    = "\0";
const char* ACTION_STR_REMOTE_START_TRANSACTION        = "\0";
const char* ACTION_STR_REMOTE_STOP_TRANSACTION         = "\0";
const char* ACTION_STR_RESERVE_NOW                     = "\0";
const char* ACTION_STR_RESET                           = "\0";
const char* ACTION_STR_SEND_LOCAL_LIST                 = "\0";
const char* ACTION_STR_SET_CHARGING_PROFILE            = "\0";
const char* ACTION_STR_START_TRANSACTION               = "\0";
const char* ACTION_STR_STATUS_NOTIFICATION             = "StatusNotification\0";
const char* ACTION_STR_STOP_TRANSACTION                = "\0";
const char* ACTION_STR_TRIGGER_MESSAGE                 = "\0";
const char* ACTION_STR_UNLOCK_CONNECTOR                = "UnlockConnector\0";
const char* ACTION_STR_UPDATE_FIRMWARE                 = "\0";

const char* OCPP_PARAM_NAME_STR_CURRENT_TIME = "currentTime\0";
const char* OCPP_PARAM_NAME_STR_INTERVAL     = "interval\0";
const char* OCPP_PARAM_NAME_STR_STATUS       = "status\0";
const char* OCPP_PARAM_NAME_STR_ID_TAG_INFO  = "idTagInfo\0";
const char* OCPP_PARAM_NAME_STR_CONNECTOR_ID = "connectorId\0";


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
const char* CHARGE_POINT_STATUS_STR_PREPARING      = "\0";
const char* CHARGE_POINT_STATUS_STR_CHARGING       = "\0";
const char* CHARGE_POINT_STATUS_STR_SUSPENDED_EVSE = "\0";
const char* CHARGE_POINT_STATUS_STR_SUSPENDED_EV   = "\0";
const char* CHARGE_POINT_STATUS_STR_FINISHING      = "\0";
const char* CHARGE_POINT_STATUS_STR_RESERVED       = "\0";
const char* CHARGE_POINT_STATUS_STR_UNAVAILABLE    = "\0";
const char* CHARGE_POINT_STATUS_STR_FAULTED        = "Faulted\0";

const char* UNLOCK_STATUS_STR_UNLOCKED      = "Unlocked\0";
const char* UNLOCK_STATUS_STR_UNLOCK_FAILED = "UnlockFailed\0";
const char* UNLOCK_STATUS_STR_NOT_SUPPORTED = "NotSupported\0";

const char *getActionString(int action){
	const char* res = EMPTY_STRING;

	switch(action){
		case ACTION_BOOT_NOTIFICATION:
			res = ACTION_STR_BOOT_NOTIFICATION;
			break;
		case ACTION_AUTHORIZE:
			res = ACTION_STR_AUTHORIZE;
			break;
		case ACTION_STATUS_NOTIFICATION:
			res = ACTION_STR_STATUS_NOTIFICATION;
			break;
		case ACTION_UNLOCK_CONNECTOR:
			res = ACTION_STR_UNLOCK_CONNECTOR;
			break;
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

const char *occpGetParamNameString(int param){
	switch(param){
		case OCPP_PARAM_CURRENT_TIME:
			return OCPP_PARAM_NAME_STR_CURRENT_TIME;
		case OCPP_PARAM_INTERVAL:
			return OCPP_PARAM_NAME_STR_INTERVAL;
		case OCPP_PARAM_STATUS:
			return OCPP_PARAM_NAME_STR_STATUS;
		case OCPP_PARAM_ID_TAG_INFO:
			return OCPP_PARAM_NAME_STR_ID_TAG_INFO;
		case OCPP_PARAM_CONNECTOR_ID:
			return OCPP_PARAM_NAME_STR_CONNECTOR_ID;
	}
	return EMPTY_STRING;
}

int occpGetActionFromString(const char* s){
	if(strcmp(s, ACTION_STR_UNLOCK_CONNECTOR) == 0)
		return ACTION_UNLOCK_CONNECTOR;
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