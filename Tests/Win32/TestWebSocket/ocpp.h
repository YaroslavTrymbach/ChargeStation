#pragma once

//
typedef char CiString20Type[21];
typedef char CiString25Type[26];

typedef CiString20Type idToken;

// Messages
#define ACTION_UNKNOWN             0
#define ACTION_AUTHORIZE           1
#define ACTION_BOOT_NOTIFICATION   2
#define ACTION_STATUS_NOTIFICATION 3
#define ACTION_UNLOCK_CONNECTOR    27

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
