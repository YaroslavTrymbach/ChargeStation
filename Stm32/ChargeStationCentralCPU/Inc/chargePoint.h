#pragma once

#ifndef _WIN32
#include <stdbool.h>
#else
#include "defTypes.h"
#endif

typedef struct _ChargePointStatus{
	int state;
	int errors;
	int lastErrorCode;
}ChargePointStatus;

typedef struct _ChargePoint{
	ChargePointStatus status;
}ChargePoint;

void ChargePoint_init(void);

int ChargePoint_getStatusState(void);
void ChargePoint_setStatusState(int state, int code, char* info);

void ChargePoint_setRFIDEnabled(bool enabled);

int ChargePoint_getStatusLastErrorCode(void);
