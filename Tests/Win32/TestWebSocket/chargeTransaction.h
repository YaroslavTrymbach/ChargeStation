#pragma once

#ifndef _WIN32
#include <stdbool.h>
#else
#include "defTypes.h"
#endif

typedef struct _ChargeTransaction{
	int id;
	bool isActive;
	bool isClosed;
	int meterValue;
	int startMeterValue;
	int stopMeterValue;
	int stopReason;
}ChargeTransaction;

void chargeTransaction_reset(ChargeTransaction *trans);
void chargeTransaction_accept(ChargeTransaction *trans, int newId);
int chargeTransaction_getId(ChargeTransaction *trans);
void chargeTransaction_setMeterValue(ChargeTransaction *trans, int value);
int chargeTransaction_getMeterValue(ChargeTransaction *trans);
