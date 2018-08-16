#pragma once

#ifndef _WIN32
#include <stdbool.h>
#else
#include "defTypes.h"
#endif

typedef struct _ChargePointConnector2{
	int address;
	int status;
	bool online;
}ChargePointConnector;
