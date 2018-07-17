#include "log.h"
#include "stm32f4xx_hal.h"
#include "string.h"

extern UART_HandleTypeDef huart3;

void logStr(char* s){
	//HAL_UART_Transmit(&huart3, (uint8_t*)s, strlen(s), 0x1000);
}
