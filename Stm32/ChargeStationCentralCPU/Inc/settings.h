#include <stdbool.h>

#define SIZE_CHARGE_POINT_ID 24

typedef struct _ChargePointSettings{
	char ChargePointId[SIZE_CHARGE_POINT_ID];
	bool isDHCPEnabled;
	unsigned char LocalIp[4];
	unsigned char NetMask[4];
	unsigned char GetewayIp[4];
}ChargePointSetting;

void Settings_init(void);
ChargePointSetting* Settings_get(void);
bool Settings_save(void);
