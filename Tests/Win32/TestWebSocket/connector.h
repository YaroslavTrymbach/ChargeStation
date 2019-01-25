#pragma once

#include "chargeTransaction.h"

#ifndef _WIN32
#include <stdbool.h>
#else
#include "defTypes.h"
#endif

typedef struct _ChargePointConnector{
	int id;
	int address;
	int status;
	bool online;
	ChargeTransaction chargeTransaction;
}ChargePointConnector;
