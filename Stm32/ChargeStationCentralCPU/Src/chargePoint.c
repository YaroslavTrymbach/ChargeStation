#include "chargePoint.h"
#include "ocpp.h"

ChargePoint chargePoint;

void setStatusError(int error){
	chargePoint.status.state = CHARGE_POINT_STATUS_FAULTED;
	chargePoint.status.lastErrorCode = error;
	chargePoint.status.errors |= (1 << error); 
}

void clearStatusError(int error){
	int i, mask;
	chargePoint.status.errors &= (~(1 << error)); 
	if(chargePoint.status.errors == 0){
		chargePoint.status.state = CHARGE_POINT_STATUS_AVAILABLE;
		chargePoint.status.lastErrorCode = CHARGE_POINT_ERROR_CODE_NO_ERROR;
	}
	else{
		mask = 1;
		for(i = 0; i < 32; i++){
			if(mask & chargePoint.status.errors){
				chargePoint.status.lastErrorCode = 0;
				break;
			}
			mask <<= 1;
		}
	}
}

void ChargePoint_init(void){
	chargePoint.status.state = CHARGE_POINT_STATUS_UNAVAILABLE;
	chargePoint.status.errors = 0;
	chargePoint.status.lastErrorCode = CHARGE_POINT_ERROR_CODE_NO_ERROR;
}

int ChargePoint_getStatusState(void){
	return chargePoint.status.state;
}

int ChargePoint_getStatusLastErrorCode(void){
	return chargePoint.status.lastErrorCode;
}

void ChargePoint_setStatusState(int state, int code, char* info){
	chargePoint.status.state = state;
}

void ChargePoint_setRFIDEnabled(bool enabled){
	int err = CHARGE_POINT_ERROR_CODE_READER_FAILURE;
	if(enabled){
		clearStatusError(err);
	}
	else{
		setStatusError(err);
	}
	
}

