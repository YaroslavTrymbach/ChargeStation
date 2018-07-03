#include "stm32f4xx_hal.h"
#include <string.h>

extern RNG_HandleTypeDef hrng;

uint32_t generateRnd32(void){
	uint32_t value;
	HAL_RNG_GenerateRandomNumber(&hrng, &value);
	return value;
}

void generateGUID(char *GUID){
	sprintf(GUID, "%.4xf3b0-c87a-4ec5-a0a4-721f5b884abe", generateRnd32());
	//strncpy(GUID, "c817f3b0-c87a-4ec5-a0a4-721f5b884abe", 36);
}
