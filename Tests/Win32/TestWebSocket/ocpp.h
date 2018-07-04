#pragma once

//
typedef char CiString20Type[21];
typedef char CiString25Type[26];

typedef CiString20Type idToken;

// Messages
#define ACTION_UNKNOWN                         0
#define ACTION_AUTHORIZE                       1
#define ACTION_BOOT_NOTIFICATION               2
#define ACTION_CANCEL_RESERVATION              3
#define ACTION_CHANGE_AVAILABILITY             4
#define ACTION_CHANGE_CONFIGURATION            5
#define ACTION_CLEAR_CACHE                     6
#define ACTION_CLEAR_CHARGING_PROFILE          7
#define ACTION_DATA_TRANSFER                   8
#define ACTION_DIAGNOSTICS_STATUS_NOTIFICATION 9 
#define ACTION_FIRMWARE_STATUS_NOTIFICATION    10 
#define ACTION_GET_COMPOSITE_SCHEDULE          11 
#define ACTION_GET_CONFIGURATION               12 
#define ACTION_GET_DIAGNOSTICS                 13
#define ACTION_GET_LOCAL_LIST_VERSION          14
#define ACTION_HEARTBEAT                       15
#define ACTION_METER_VALUES                    16
#define ACTION_REMOTE_START_TRANSACTION        17
#define ACTION_REMOTE_STOP_TRANSACTION         18
#define ACTION_RESERVE_NOW                     19
#define ACTION_RESET                           20
#define ACTION_SEND_LOCAL_LIST                 21
#define ACTION_SET_CHARGING_PROFILE            22
#define ACTION_START_TRANSACTION               23
#define ACTION_STATUS_NOTIFICATION             24 
#define ACTION_STOP_TRANSACTION                25
#define ACTION_TRIGGER_MESSAGE                 26
#define ACTION_UNLOCK_CONNECTOR                27
#define ACTION_UPDATE_FIRMWARE                 28

#define AUTHORIZATION_STATUS_UNKNOWN       0
#define AUTHORIZATION_STATUS_ACCEPTED      1
#define AUTHORIZATION_STATUS_BLOCKED       2
#define AUTHORIZATION_STATUS_EXPIRED       3
#define AUTHORIZATION_STATUS_INVALID       4
#define AUTHORIZATION_STATUS_CONCURRENT_TX 5

#define REGISTRATION_STATUS_UNKNOWN  0
#define REGISTRATION_STATUS_ACCEPTED 1
#define REGISTRATION_STATUS_PENDING  2
#define REGISTRATION_STATUS_REJECTED 3

#define CHARGE_POINT_STATUS_UNKNOWN        0
#define CHARGE_POINT_STATUS_AVAILABLE      1
#define CHARGE_POINT_STATUS_PREPARING      2
#define CHARGE_POINT_STATUS_CHARGING       3
#define CHARGE_POINT_STATUS_SUSPENDED_EVSE 4
#define CHARGE_POINT_STATUS_SUSPENDED_EV   5
#define CHARGE_POINT_STATUS_FINISHING      6
#define CHARGE_POINT_STATUS_RESERVED       7
#define CHARGE_POINT_STATUS_UNAVAILABLE    8
#define CHARGE_POINT_STATUS_FAULTED        9

#define CHARGE_POINT_ERROR_CODE_UNKNOWN                0
#define CHARGE_POINT_ERROR_CODE_CONNECTOR_LOCK_FAILURE 1
#define CHARGE_POINT_ERROR_CODE_NO_ERROR               7

#define UNLOCK_STATUS_UNKNOWN       0
#define UNLOCK_STATUS_UNLOCKED      1
#define UNLOCK_STATUS_UNLOCK_FAILED 2
#define UNLOCK_STATUS_NOT_SUPPORTED 3

typedef struct _RequestBootNotification {
   CiString20Type chargePointModel;
   CiString20Type chargePointVendor;
}RequestBootNotification;

typedef struct _RequestAuthorize{
   idToken idTag;
}RequestAuthorize;

typedef struct _RequestStatusNotification{
	int connectorId;
    int	errorCode;
	int status;
}RequestStatusNotification;

typedef struct _RequestUnlockConnector{
	int connectorId;
}RequestUnlockConnector;

typedef struct _IdTagInfo{
	int expiryDate;
	idToken parentIdTag;
	int status;

}IdTagInfo;

typedef struct _ConfAuthorize{
	IdTagInfo idTagInfo;
}ConfAuthorize;

typedef struct _ConfBootNotifiaction{
	int currentTime;
	int interval;
	int status;
}ConfBootNotifiaction;

typedef struct _ConfUnlockConnector{
	int status;
}ConfUnlockConnector;


#define OCPP_PARAM_CURRENT_TIME  0
#define OCPP_PARAM_INTERVAL      1
#define OCPP_PARAM_STATUS        2
#define OCPP_PARAM_ID_TAG_INFO   3
#define OCPP_PARAM_CONNECTOR_ID  4

const char *getActionString(int action);
const char *getChargePointErrorCodeString(int errorCode);
const char *getChargePointStatusString(int status);
const char *getUnlockStatusString(int status);

const char *occpGetParamNameString(int param);

int occpGetActionFromString(const char* s);
int occpGetRegistrationStatusFromString(const char* s);
int occpGetAuthorizationStatusFromString(const char* s);