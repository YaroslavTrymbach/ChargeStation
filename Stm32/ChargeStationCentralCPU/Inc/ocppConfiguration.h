#pragma once

#ifndef _WIN32
#include <stdbool.h>
#else
#include "defTypes.h"
#endif

#define CONFIG_KEY_UNKNOWN                            0
#define CONFIG_KEY_ALLOW_OFLINE_TX_FOR_UNKNOWN_ID     1
#define CONFIG_KEY_AUTHORIZATION_CACHE_ENABLED        2
#define CONFIG_KEY_AUTHORIZE_REMOTE_TX_REQUESTS       3
#define CONFIG_KEY_BLINK_REPEAT                       4
#define CONFIG_KEY_CLOCK_ALIGNED_DATA_INTERVAL        5
#define CONFIG_KEY_CONNECTION_TIMEOUT                 6
#define CONFIG_KEY_CONNECTOR_PHASE_ROTATION           7
#define CONFIG_KEY_CONNECTOR_PHASE_ROTATION_MAX_LENGH 8
#define CONFIG_KEY_GET_CONFIGURATION_MAX_KEYS         9
#define CONFIG_KEY_HEARTBEAT_INTERVAL                 10
#define CONFIG_KEY_LIGHT_INTENSITY                    11

// Struct with ocpp configuration that can be changed via server
typedef struct _OcppConfigurationVaried{
	bool authorizeRemoteTxRequests;
}OcppConfigurationVaried;

// Struct with ocpp configuration that cannot be changed via server
typedef struct _OcppConfigurationFixed{
	bool authorizeRemoteTxRequests;
}OcppConfigurationFixed;

// Struct with control wether configuration key can be changed via server
typedef struct _OcppConfigurationRestrict{
	bool authorizeRemoteTxRequestsReadOnly;
}OcppConfigurationRestrict;

const char *ocppGetConfigKeyString(int configKey);
int occpGetConfigKeyFromString(const char* s);
