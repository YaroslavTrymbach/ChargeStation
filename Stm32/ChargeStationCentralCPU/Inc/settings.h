#include <stdbool.h>

#define SIZE_CHARGE_POINT_ID 24
#define SIZE_SERVER_HOST     64
#define SIZE_SERVER_URI      128

typedef struct _ChargePointSettings{
	char ChargePointId[SIZE_CHARGE_POINT_ID];
	bool isDHCPEnabled;
	unsigned char LocalIp[4];
	unsigned char NetMask[4];
	unsigned char GetewayIp[4];
	char serverHost[SIZE_SERVER_HOST];
	char serverUri[SIZE_SERVER_URI];
	int serverPort;
}ChargePointSetting;

void Settings_init(void);
ChargePointSetting* Settings_get(void);
bool Settings_save(void);
