#include "chargePointState.h"
#include "netEth.h"

typedef struct _TChargePointState{
	bool isServerOnline;
	bool isNetworkOnline;
}TChargePointState;

static TChargePointState chargePointState;

bool cpState_isServerOnline(void){
	return chargePointState.isServerOnline;
}

void cpState_setServerOnline(bool online){
	chargePointState.isServerOnline = online;
}

bool cpState_isNetworkOnline(void){
	return NET_ETH_check_link_up();
	//return chargePointState.isNetworkOnline;
}

void cpState_setNetworkOnline(bool online){
	chargePointState.isNetworkOnline = online;
}
