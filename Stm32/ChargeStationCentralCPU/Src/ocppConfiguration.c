#include "ocppConfiguration.h"

#include "tools.h"
#include "string.h"

const char* CONFIG_KEY_STR_ALLOW_OFLINE_TX_FOR_UNKNOWN_ID     = "AllowOfflineTxForUnknownId";
const char* CONFIG_KEY_STR_AUTHORIZATION_CACHE_ENABLED        = "AuthorizationCacheEnabled";
const char* CONFIG_KEY_STR_AUTHORIZE_REMOTE_TX_REQUESTS       = "AuthorizeRemoteTxRequests";
const char* CONFIG_KEY_STR_BLINK_REPEAT                       = "BlinkRepeat";
const char* CONFIG_KEY_STR_CLOCK_ALIGNED_DATA_INTERVAL        = "ClockAlignedDataInterval";
const char* CONFIG_KEY_STR_CONNECTION_TIMEOUT                 = "ConnectionTimeOut";
const char* CONFIG_KEY_STR_CONNECTOR_PHASE_ROTATION           = "ConnectorPhaseRotation";
const char* CONFIG_KEY_STR_CONNECTOR_PHASE_ROTATION_MAX_LENGH = "ConnectorPhaseRotationMaxLength";
const char* CONFIG_KEY_STR_GET_CONFIGURATION_MAX_KEYS         = "GetConfigurationMaxKeys";
const char* CONFIG_KEY_STR_HEARTBEAT_INTERVAL                 = "HeartbeatInterval";
const char* CONFIG_KEY_STR_LIGHT_INTENSITY                    = "LightIntensity";
const char* CONFIG_KEY_STR_LOCAL_AUTHORIZE_OFFLINE                = "LocalAuthorizeOffline";
const char* CONFIG_KEY_STR_LOCAL_PRE_AUTHORIZE                    = "LocalPreAuthorize";
const char* CONFIG_KEY_STR_MAX_ENERGY_ON_INVALID_ID               = "MaxEnergyOnInvalidId";
const char* CONFIG_KEY_STR_METER_VALUES_ALIGNED_DATA              = "MeterValuesAlignedData";
const char* CONFIG_KEY_STR_METER_VALUES_ALIGNED_DATA_MAX_LENGTH   = "MeterValuesAlignedDataMaxLength";
const char* CONFIG_KEY_STR_METER_VALUES_SAMPLED_DATA              = "MeterValuesSampledData";
const char* CONFIG_KEY_STR_METER_VALUES_SAMPLED_DATA_MAX_LENGTH   = "MeterValuesSampledDataMaxLength";
const char* CONFIG_KEY_STR_METER_VALUES_SAMPLE_INTERVAL           = "MeterValueSampleInterval";
const char* CONFIG_KEY_STR_MINIMUM_STATUS_DURATION                = "MinimumStatusDuration";
const char* CONFIG_KEY_STR_NUMBER_OF_CONNECTORS                   = "NumberOfConnectors";
const char* CONFIG_KEY_STR_RESET_RETRIES                          = "ResetRetries";
const char* CONFIG_KEY_STR_STOP_TRANSACTION_ON_EV_SIDE_DISCONNECT = "StopTransactionOnEVSideDisconnect";
const char* CONFIG_KEY_STR_STOP_TRANSACTION_ON_INVALID_ID         = "StopTransactionOnInvalidId";
const char* CONFIG_KEY_STR_STOP_TXN_ALIGNED_DATA                  = "StopTxnAlignedData";
const char* CONFIG_KEY_STR_STOP_TXN_ALIGNED_DATA_MAX_LENGTH       = "StopTxnAlignedDataMaxLength";
const char* CONFIG_KEY_STR_STOP_TXN_SAMPLED_DATA                  = "StopTxnSampledData";
const char* CONFIG_KEY_STR_STOP_TXN_SAMPLED_DATA_MAX_LENGTH       = "StopTxnSampledDataMaxLength";
const char* CONFIG_KEY_STR_SUPPORTED_FEATURE_PROFILES             = "SupportedFeatureProfiles";
const char* CONFIG_KEY_STR_SUPPORTED_FEATURE_PROFILES_MAX_LENGTH  = "SupportedFeatureProfilesMaxLength";
const char* CONFIG_KEY_STR_TRANSACTION_MESSAGE_ATTEMPTS           = "TransactionMessageAttempts";
const char* CONFIG_KEY_STR_TRANSACTION_MESSAGE_RETRY_INTERVAL     = "TransactionMessageRetryInterval";
const char* CONFIG_KEY_STR_UNLOCK_CONNECTOR_ON_EV_SIDE_DISCONNECT = "UnlockConnectorOnEVSideDisconnect";
const char* CONFIG_KEY_STR_WEB_SOCKET_PING_INTERVAL               = "WebScoketPingInterval";
const char* CONFIG_KEY_STR_LOCAL_AUTH_LIST_ENABLED                = "LocalAuthListEnabled";
const char* CONFIG_KEY_STR_LOCAL_AUTH_LIST_MAX_LENGTH             = "LocalAuthListMaxLength";
const char* CONFIG_KEY_STR_SEND_LOCAL_LIST_MAX_LENGTH             = "SendLocalListMaxLength";
//const char* CONFIG_KEY_STR_ = "";


extern const char* EMPTY_STRING;

const char *ocppGetConfigKeyString(int configKey){

#define CASE_CONFIG_KEY_STR(name) case CONFIG_KEY_##name: \
	                                res = CONFIG_KEY_STR_##name; \
								    break

	const char* res = EMPTY_STRING;

	switch(configKey){	
		CASE_CONFIG_KEY_STR(ALLOW_OFLINE_TX_FOR_UNKNOWN_ID);
		CASE_CONFIG_KEY_STR(AUTHORIZATION_CACHE_ENABLED);
		CASE_CONFIG_KEY_STR(AUTHORIZE_REMOTE_TX_REQUESTS);
		CASE_CONFIG_KEY_STR(BLINK_REPEAT);
		CASE_CONFIG_KEY_STR(CLOCK_ALIGNED_DATA_INTERVAL);
		CASE_CONFIG_KEY_STR(CONNECTION_TIMEOUT);
		CASE_CONFIG_KEY_STR(CONNECTOR_PHASE_ROTATION);
		CASE_CONFIG_KEY_STR(CONNECTOR_PHASE_ROTATION_MAX_LENGH);
		CASE_CONFIG_KEY_STR(GET_CONFIGURATION_MAX_KEYS);
		CASE_CONFIG_KEY_STR(HEARTBEAT_INTERVAL);
		CASE_CONFIG_KEY_STR(LIGHT_INTENSITY);
		CASE_CONFIG_KEY_STR(LOCAL_AUTHORIZE_OFFLINE);
		CASE_CONFIG_KEY_STR(LOCAL_PRE_AUTHORIZE);
		CASE_CONFIG_KEY_STR(MAX_ENERGY_ON_INVALID_ID);
		CASE_CONFIG_KEY_STR(METER_VALUES_ALIGNED_DATA);
		CASE_CONFIG_KEY_STR(METER_VALUES_ALIGNED_DATA_MAX_LENGTH);
		CASE_CONFIG_KEY_STR(METER_VALUES_SAMPLED_DATA);
		CASE_CONFIG_KEY_STR(METER_VALUES_SAMPLED_DATA_MAX_LENGTH);
		CASE_CONFIG_KEY_STR(METER_VALUES_SAMPLE_INTERVAL);
		CASE_CONFIG_KEY_STR(MINIMUM_STATUS_DURATION);
		CASE_CONFIG_KEY_STR(NUMBER_OF_CONNECTORS);
		CASE_CONFIG_KEY_STR(RESET_RETRIES);
		CASE_CONFIG_KEY_STR(STOP_TRANSACTION_ON_EV_SIDE_DISCONNECT);
		CASE_CONFIG_KEY_STR(STOP_TRANSACTION_ON_INVALID_ID);
		CASE_CONFIG_KEY_STR(STOP_TXN_ALIGNED_DATA);
		CASE_CONFIG_KEY_STR(STOP_TXN_ALIGNED_DATA_MAX_LENGTH);
		CASE_CONFIG_KEY_STR(STOP_TXN_SAMPLED_DATA);
		CASE_CONFIG_KEY_STR(STOP_TXN_SAMPLED_DATA_MAX_LENGTH);
		CASE_CONFIG_KEY_STR(SUPPORTED_FEATURE_PROFILES);
		CASE_CONFIG_KEY_STR(SUPPORTED_FEATURE_PROFILES_MAX_LENGTH);
		CASE_CONFIG_KEY_STR(TRANSACTION_MESSAGE_ATTEMPTS);
		CASE_CONFIG_KEY_STR(TRANSACTION_MESSAGE_RETRY_INTERVAL);
		CASE_CONFIG_KEY_STR(UNLOCK_CONNECTOR_ON_EV_SIDE_DISCONNECT);
		CASE_CONFIG_KEY_STR(WEB_SOCKET_PING_INTERVAL);
		
		CASE_CONFIG_KEY_STR(LOCAL_AUTH_LIST_ENABLED);
		CASE_CONFIG_KEY_STR(LOCAL_AUTH_LIST_MAX_LENGTH);
		CASE_CONFIG_KEY_STR(SEND_LOCAL_LIST_MAX_LENGTH);
		//CASE_CONFIG_KEY_STR();
	}
	return res;
}


int occpGetConfigKeyFromString(const char* s){

#define CHECK_CONFIG_KEY(name) if(strcmp(s, CONFIG_KEY_STR_##name) == 0) \
		                     return CONFIG_KEY_##name;

	CHECK_CONFIG_KEY(ALLOW_OFLINE_TX_FOR_UNKNOWN_ID);
	CHECK_CONFIG_KEY(AUTHORIZATION_CACHE_ENABLED);
	CHECK_CONFIG_KEY(AUTHORIZE_REMOTE_TX_REQUESTS);
	CHECK_CONFIG_KEY(BLINK_REPEAT);
	CHECK_CONFIG_KEY(CLOCK_ALIGNED_DATA_INTERVAL);
	CHECK_CONFIG_KEY(CONNECTION_TIMEOUT);
	CHECK_CONFIG_KEY(CONNECTOR_PHASE_ROTATION);
	CHECK_CONFIG_KEY(CONNECTOR_PHASE_ROTATION_MAX_LENGH);
	CHECK_CONFIG_KEY(GET_CONFIGURATION_MAX_KEYS);
	CHECK_CONFIG_KEY(HEARTBEAT_INTERVAL);
	CHECK_CONFIG_KEY(LIGHT_INTENSITY);
	CHECK_CONFIG_KEY(LOCAL_AUTHORIZE_OFFLINE);
	CHECK_CONFIG_KEY(LOCAL_PRE_AUTHORIZE);
	CHECK_CONFIG_KEY(MAX_ENERGY_ON_INVALID_ID);
	CHECK_CONFIG_KEY(METER_VALUES_ALIGNED_DATA);
	CHECK_CONFIG_KEY(METER_VALUES_ALIGNED_DATA_MAX_LENGTH);
	CHECK_CONFIG_KEY(METER_VALUES_SAMPLED_DATA);
	CHECK_CONFIG_KEY(METER_VALUES_SAMPLED_DATA_MAX_LENGTH);
	CHECK_CONFIG_KEY(METER_VALUES_SAMPLE_INTERVAL);
	CHECK_CONFIG_KEY(MINIMUM_STATUS_DURATION);
	CHECK_CONFIG_KEY(NUMBER_OF_CONNECTORS);
	CHECK_CONFIG_KEY(RESET_RETRIES);
	CHECK_CONFIG_KEY(STOP_TRANSACTION_ON_EV_SIDE_DISCONNECT);
	CHECK_CONFIG_KEY(STOP_TRANSACTION_ON_INVALID_ID);
	CHECK_CONFIG_KEY(STOP_TXN_ALIGNED_DATA);
	CHECK_CONFIG_KEY(STOP_TXN_ALIGNED_DATA_MAX_LENGTH);
	CHECK_CONFIG_KEY(STOP_TXN_SAMPLED_DATA);
	CHECK_CONFIG_KEY(STOP_TXN_SAMPLED_DATA_MAX_LENGTH);
	CHECK_CONFIG_KEY(SUPPORTED_FEATURE_PROFILES);
	CHECK_CONFIG_KEY(SUPPORTED_FEATURE_PROFILES_MAX_LENGTH);
	CHECK_CONFIG_KEY(TRANSACTION_MESSAGE_ATTEMPTS);
	CHECK_CONFIG_KEY(TRANSACTION_MESSAGE_RETRY_INTERVAL);
	CHECK_CONFIG_KEY(UNLOCK_CONNECTOR_ON_EV_SIDE_DISCONNECT);
	CHECK_CONFIG_KEY(WEB_SOCKET_PING_INTERVAL);
	CHECK_CONFIG_KEY(LOCAL_AUTH_LIST_ENABLED);
	CHECK_CONFIG_KEY(LOCAL_AUTH_LIST_MAX_LENGTH);
	CHECK_CONFIG_KEY(SEND_LOCAL_LIST_MAX_LENGTH);
	//CHECK_CONFIG_KEY();

	return CONFIG_KEY_UNKNOWN;
}

bool ocppGetConfigValueFromStringInt(const char* s, int *value){
	int outValue;
	if(strToIntWithTrim(s, &outValue)){
		*value = outValue;
		return true;
	}
	return false;
}

bool ocppGetConfigValueFromStringBool(const char* s, bool *value){
	if(strcmp(s, "true") == 0){
		*value = true;
		return true;
	}
	else if(strcmp(s, "false") == 0){
		*value = false;
		return true;
	}
	return false;
}
