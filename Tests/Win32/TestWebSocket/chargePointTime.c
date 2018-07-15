#include "chargePointTime.h"

bool getCurrentTime(struct tm* time){
	time_t lt;
	_time64(&lt);
	localtime_s(time, &lt);
	time->tm_mon += 1;
	time->tm_year = (time->tm_year % 100) + 2000;
	return true;
}

bool setCurrentTime(struct tm* time){
	return true;
}