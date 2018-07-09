#include "chargePointTime.h"

bool getCurrentTime(struct tm* time){
	time_t lt;
	_time64(&lt);
	localtime_s(time, &lt);
	return true;
}

bool setCurrentTime(struct tm* time){
	return true;
}