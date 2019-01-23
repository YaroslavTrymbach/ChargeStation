#include "chargeTransaction.h"

void chargeTransaction_reset(ChargeTransaction *trans){
	trans->id = 0;
	trans->isActive = false;
}

void chargeTransaction_accept(ChargeTransaction *trans, int newId){
	trans->isActive = true;
	trans->id = newId;
}

int chargeTransaction_getId(ChargeTransaction *trans){
	return trans->id;
}

void chargeTransaction_setMeterValue(ChargeTransaction *trans, int value){
	trans->meterValue = value;
}

int chargeTransaction_getMeterValue(ChargeTransaction *trans){
	return trans->meterValue;
}