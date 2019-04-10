#pragma once

#include "chargeTransaction.h"
#include "ocpp.h"

#ifndef _WIN32
#include <stdbool.h>
#else
#include "defTypes.h"
#endif

#define PILOT_SYGNAL_LEVEL_A 0 // 12 V
#define PILOT_SYGNAL_LEVEL_B 1 // 9 V
#define PILOT_SYGNAL_LEVEL_C 2 // 6 V
#define PILOT_SYGNAL_LEVEL_D 3 // 3 V
#define PILOT_SYGNAL_LEVEL_E 4 // 0 V
#define PILOT_SYGNAL_LEVEL_F 5 // N/A

#define PILOT_SYGNAL_PWM_NONE    0
#define PILOT_SYGNAL_PWM_7kWt    1
#define PILOT_SYGNAL_PWM_22kWt   2
#define PILOT_SYGNAL_PWM_UNKNOWN 3

#define START_CHAR_SUCCESS '!'
#define START_CHAR_ERROR   '?'

typedef struct _ChargePointConnector{
	int id;
	int address;
	int status;
	bool online;
	idToken userTagId;
	int meterValue;
	int meterValueError;
	bool isMeterValueSet;
	bool isMeterValueRequest; //Request for getting meterValue from channel controller
	bool isNeedStartCharging;
	bool isNeedHaltCharging;
	bool isNeedUnlockConnector;
	bool isNeedReset;
	int uniqMesIndUnlockConnector;
	int pilotSygnalLevel;
	int pilotSygnalPwm;
	ChargeTransaction chargeTransaction;
	int errorCode;
	int noAnswerCnt;
	bool isLocked;
}ChargePointConnector;
