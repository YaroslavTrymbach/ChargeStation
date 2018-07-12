#include "chargePointTime.h"

#include "stm32f4xx_hal.h"

extern RTC_HandleTypeDef hrtc;

bool getCurrentTime(struct tm* time){
	RTC_TimeTypeDef t;
	RTC_DateTypeDef d;
	
	HAL_RTC_GetTime(&hrtc, &t, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &d, RTC_FORMAT_BIN);
	
	time->tm_year = d.Year + 2000;
	time->tm_mon = d.Month;
	time->tm_mday = d.Date;
	
	time->tm_hour = t.Hours;
	time->tm_min = t.Minutes;
	time->tm_sec = t.Seconds;
	return true;
}

bool setCurrentTime(struct tm* time){
	RTC_TimeTypeDef t;
	RTC_DateTypeDef d;
	
	t.Hours = time->tm_hour;
	t.Minutes = time->tm_min;
	t.Seconds = time->tm_sec;
	t.TimeFormat = RTC_HOURFORMAT12_AM;
	
	HAL_RTC_SetTime(&hrtc, &t, RTC_FORMAT_BIN);
	
	d.Year = time->tm_year % 100;
	d.Month = time->tm_mon;
	d.Date = time->tm_mday;
	d.WeekDay = RTC_WEEKDAY_SUNDAY; //let there always be SUNDAY)
	
	HAL_RTC_SetDate(&hrtc, &d, RTC_FORMAT_BIN);
	
	return true;
}
