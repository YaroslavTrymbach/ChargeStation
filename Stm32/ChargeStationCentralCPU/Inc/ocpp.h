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

#define CONFIGURATION_STATUS_ACCEPTED        0
#define CONFIGURATION_STATUS_REJECTED        1
#define CONFIGURATION_STATUS_REBOOT_REQUIRED 2
#define CONFIGURATION_STATUS_NOT_SUPPORTED   3

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
#define CHARGE_POINT_ERROR_CODE_EV_COMMUNICATION_ERROR 2
#define CHARGE_POINT_ERROR_CODE_GROUND_FAILURE         3
#define CHARGE_POINT_ERROR_CODE_HIGH_TEMPERATURE       4
#define CHARGE_POINT_ERROR_CODE_INTERNAL_ERROR         5 
#define CHARGE_POINT_ERROR_CODE_LOCAL_LIST_CONFLICT    6
#define CHARGE_POINT_ERROR_CODE_NO_ERROR               7
#define CHARGE_POINT_ERROR_CODE_OTHER_ERROR            8
#define CHARGE_POINT_ERROR_CODE_OVER_CURRENT_FAILURE   9
#define CHARGE_POINT_ERROR_CODE_OVER_VOLTAGE           10
#define CHARGE_POINT_ERROR_CODE_POWER_METER_FAILURE    11 
#define CHARGE_POINT_ERROR_CODE_POWER_SWITCH_FAILURE   12
#define CHARGE_POINT_ERROR_CODE_READER_FAILURE         13
#define CHARGE_POINT_ERROR_CODE_RESET_FAILURE          14
#define CHARGE_POINT_ERROR_CODE_UNDER_VOLTAGE          15
#define CHARGE_POINT_ERROR_CODE_WEAK_SIGNAL            16

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
#define OCPP_PARAM_METER_VALUE       18
#define OCPP_PARAM_SAMPLED_VALUE     19
#define OCPP_PARAM_LIST_VERSION      20

#define OCPP_NUMBER_PROFILE 6

#define OCPP_PROFILE_CORE                       0
#define OCPP_PROFILE_FIRMWARE_MANAGEMENT        1
#define OCPP_PROFILE_LOCAL_AUTH_LIST_MANAGEMENT 2
#define OCPP_PROFILE_RESERVATION                3
#define OCPP_PROFILE_SMART_CHARGING             4
#define OCPP_PROFILE_REMOTE_TRIGGER             5

#define OCPP_PROFILE_MASK_CORE                       (1 << OCPP_PROFILE_CORE)
#define OCPP_PROFILE_MASK_FIRMWARE_MANAGEMENT        (1 << OCPP_PROFILE_FIRMWARE_MANAGEMENT)
#define OCPP_PROFILE_MASK_LOCAL_AUTH_LIST_MANAGEMENT (1 << OCPP_PROFILE_LOCAL_AUTH_LIST_MANAGEMENT)
#define OCPP_PROFILE_MASK_RESERVATION                (1 << OCPP_PROFILE_RESERVATION)
#define OCPP_PROFILE_MASK_SMART_CHARGING             (1 << OCPP_PROFILE_SMART_CHARGING)
#define OCPP_PROFILE_MASK_REMOTE_TRIGGER             (1 << OCPP_PROFILE_REMOTE_TRIGGER)

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
	CiString500Type value;
	bool vauleIsSet;
}KeyValue;

typedef struct _KeyValueListItem{
	KeyValue data;
	struct _KeyValueListItem *next;
}KeyValueListItem;

typedef struct _SampledValueListItem{
	int value;
	struct _SampledValueListItem *next;
}SampledValueListItem;

typedef struct _MeterValue{
	dateTime timestamp;
	SampledValueListItem *samledValue;
}MeterValue;

typedef struct _MeterValueListItem{
	MeterValue meterValue;
	struct _MeterValueListItem *next;
}MeterValueListItem;

typedef struct _RequestBootNotification {
   CiString20Type chargePointModel;
   CiString20Type chargePointVendor;
}RequestBootNotification;

typedef struct _RequestChangeConfiguration{
	CiString50Type key;
	CiString500Type value;
}RequestChangeConfiguration;

typedef struct _RequestGetConfiguration{
	int keySize;
	CiString50Type key[CONFIGURATION_GET_MAX_KEYS];
}RequestGetConfiguration;

typedef struct _RequestMeterValues{
	int connectorId;
	int transactionId;
	bool useTransactionId;
	MeterValueListItem *meterValue;

}RequestMeterValues;

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

typedef struct _AuthorizationData{
	idToken idTag;
	IdTagInfo idTagInfo;
}AuthorizationData;

typedef struct _ConfAuthorize{
	IdTagInfo idTagInfo;
}ConfAuthorize;

typedef struct _ConfBootNotifiaction{
	dateTime currentTime;
	int interval;
	int status;
}ConfBootNotifiaction;

typedef struct _ConfChangeConfiguration{
	int status;
}ConfChangeConfiguration;

typedef struct _ConfGetConfiguration{
	KeyValueListItem *configurationKey;
	CiString50TypeListItem *unknownKey;
}ConfGetConfiguration;

typedef struct _ConfGetLocalListVersion{
	int listVersion;
}ConfGetLocalListVersion;

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
const char *getConfigurationStatusString(int status);
const char *ocppGetProfileString(int profile);
const char *ocppGetAuthorizationStatusString(int status);

const char *ocppGetParamNameString(int param);

int ocppGetActionFromString(const char* s);
int occpConfigKeyFromString(const char* s);
int occpGetRegistrationStatusFromString(const char* s);
int occpGetAuthorizationStatusFromString(const char* s);
int occpGetResetTypeFromString(const char* s);

void ocppFreeCiString50TypeList(CiString50TypeListItem *list);
void ocppFreeKeyValueList(KeyValueListItem *list);
void ocppFreeMeterValueList(MeterValueListItem *list);
void ocppFreeSampledValueList(SampledValueListItem *list);

//Add element to end of list
void ocppAddSampledValue(MeterValue *meterValue, SampledValueListItem *value);

KeyValueListItem* ocppCreateKeyValueInt(int key, bool readonly, int value);
KeyValueListItem* ocppCreateKeyValueBool(int key, bool readonly, bool value);
KeyValueListItem* ocppCreateKeyValueString(int key, bool readonly, char* value);
MeterValueListItem* ocppCreateMeterValueItem(void);
SampledValueListItem* ocppCreateSampledValueItem(void);

char* ocppCreateProfileCSL(int mask);
