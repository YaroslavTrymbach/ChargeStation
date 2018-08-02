#pragma once

#ifndef _WIN32
#include <stdbool.h>
#else
#include "defTypes.h"
#endif

#include <time.h>

//
typedef char CiString20Type[21];
typedef char CiString25Type[26];
typedef char CiString50Type[51];
//typedef char CiString500Type[501];
typedef char* CiString500Type;
typedef struct tm dateTime;

typedef CiString20Type idToken;

//Configuration constant
#define CONFIGURATION_GET_MAX_KEYS 16
#define CONFIGURATION_NUMBER_OF_CONNECTORS 4

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

#define RESET_TYPE_UNKNOWN 0
#define RESET_TYPE_SOFT    1
#define RESET_TYPE_HARD    2

#define OCPP_PARAM_CURRENT_TIME      0
#define OCPP_PARAM_INTERVAL          1
#define OCPP_PARAM_STATUS            2
#define OCPP_PARAM_ID_TAG_INFO       3
#define OCPP_PARAM_CONNECTOR_ID      4
#define OCPP_PARAM_ID_TAG            5
#define OCPP_PARAM_METER_START       6
#define OCPP_PARAM_RESERVATION_ID    7
#define OCPP_PARAM_TIMESTAMP         8
#define OCPP_PARAM_TRANSACTION_ID    9
#define OCPP_PARAM_METER_STOP        10
#define OCPP_PARAM_REASON            11
#define OCPP_PARAM_KEY               12
#define OCPP_PARAM_UNKNOWN_KEY       13
#define OCPP_PARAM_CONFIGURATION_KEY 14
#define OCPP_PARAM_VALUE             15
#define OCPP_PARAM_READONLY          16
#define OCPP_PARAM_TYPE              17

typedef struct _CiString50TypeListItem{
	CiString50Type data;
	struct _CiString50TypeListItem *next;
}CiString50TypeListItem;

typedef struct _RequestAuthorize{
   idToken idTag;
}RequestAuthorize;

typedef struct _KeyValue{
	CiString50Type key;
	bool readonly;
	//CiString500Type *value;
	CiString500Type value;
	bool vauleIsSet;
}KeyValue;

typedef struct _KeyValueListItem{
	KeyValue data;
	struct _KeyValueListItem *next;
}KeyValueListItem;

typedef struct _RequestBootNotification {
   CiString20Type chargePointModel;
   CiString20Type chargePointVendor;
}RequestBootNotification;

typedef struct _RequestGetConfiguration{
	int keySize;
	CiString50Type key[CONFIGURATION_GET_MAX_KEYS];
}RequestGetConfiguration;

typedef struct _RequestStartTransaction{
	int connectorId;
	idToken idTag;
	int meterStart;
	dateTime timestamp;
	int reservationId;
	bool useReservationId;
}RequestStartTransaction;

typedef struct _RequestStatusNotification{
	int connectorId;
    int	errorCode;
	int status;
}RequestStatusNotification;

typedef struct _RequestStopTransaction{
	idToken idTag;
	int meterStop;
	dateTime timestamp;
	int transactionId;
	int reason;
	//MeterValue transactionData;
	bool useIdTag;
	bool useReason;
}RequestStopTransaction;

typedef struct _RequestUnlockConnector{
	int connectorId;
}RequestUnlockConnector;

typedef struct _RequestReset{
	int type;
}RequestReset;

typedef struct _RequestReserveNow{
	int connectorId;
	dateTime expiryDate;
	idToken idTag;
	idToken parentIdTag;
	int reservationId;
	bool useParentIdTag;
}RequestReserveNow;

typedef struct _RequestSendLocalList{
	int listVersion;
	int updateType;
	//AuthorizationData localAuthorizationList;
}RequestSendLocalList;

typedef struct _IdTagInfo{
	dateTime expiryDate;
	idToken parentIdTag;
	int status;

}IdTagInfo;

typedef struct _ConfAuthorize{
	IdTagInfo idTagInfo;
}ConfAuthorize;

typedef struct _ConfBootNotifiaction{
	dateTime currentTime;
	int interval;
	int status;
}ConfBootNotifiaction;

typedef struct _ConfGetConfiguration{
	KeyValueListItem *configurationKey;
	CiString50TypeListItem *unknownKey;
}ConfGetConfiguration;

typedef struct _ConfHeartbeat{
	dateTime currentTime;
}ConfHeartbeat;

typedef struct _ConfStartTransaction{
	IdTagInfo idTagInfo;
	int transactionId;
}ConfStartTransaction;

typedef struct _ConfStopTransaction{
	IdTagInfo idTagInfo;
	bool useIdTagInfo;
}ConfStopTransaction;

typedef struct _ConfUnlockConnector{
	int status;
}ConfUnlockConnector;




const char *getActionString(int action);
const char *getChargePointErrorCodeString(int errorCode);
const char *getChargePointStatusString(int status);
const char *getUnlockStatusString(int status);

const char *ocppGetParamNameString(int param);

int occpGetActionFromString(const char* s);
int occpConfigKeyFromString(const char* s);
int occpGetRegistrationStatusFromString(const char* s);
int occpGetAuthorizationStatusFromString(const char* s);
int occpGetResetTypeFromString(const char* s);

void occpFreeCiString50TypeList(CiString50TypeListItem *list);
void occpFreeKeyValueList(KeyValueListItem *list);

KeyValueListItem* occpCreateKeyValueInt(int key, bool readonly, int value);
KeyValueListItem* occpCreateKeyValueBool(int key, bool readonly, bool value);
