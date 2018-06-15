#include "display.h"
#include "lcd_hd44780_i2c.h"
#include <string.h>

#define DISPLAY_I2C_ADDRESS 0x27
#define DISPLAY_SIZE_Y      4
#define DISPLAY_SIZE_X      20

bool Display_init(I2C_HandleTypeDef *hi2c){
	lcdInit(hi2c, DISPLAY_I2C_ADDRESS, DISPLAY_SIZE_Y , DISPLAY_SIZE_X);
	return true;
}

bool Display_clear(void){
	return true;
}

bool Display_PrintStrCenter(uint8_t raw, char *data){
	int len, pos;
	char printStr[DISPLAY_SIZE_X];
	
	if(raw >= DISPLAY_SIZE_Y)
		return false;
	
	
	
	len = strlen(data);
	if(len > DISPLAY_SIZE_X){
		pos = (len - DISPLAY_SIZE_X) >> 1;
		memcpy(printStr, data + pos, DISPLAY_SIZE_X); 
	}
	else if(len < DISPLAY_SIZE_X){
		pos = (DISPLAY_SIZE_X - len) >> 1;
		memset(printStr, ' ', DISPLAY_SIZE_X);
		memcpy(printStr + pos, data, len); 
	}
	else{
		memcpy(printStr, data, DISPLAY_SIZE_X); 
	}	
	
	lcdSetCursorPosition(0, raw);
	lcdPrintStr((uint8_t*)printStr, DISPLAY_SIZE_X);
	
	return true;
}
