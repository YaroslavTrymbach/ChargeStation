#include "display.h"
#include "lcd_hd44780_i2c.h"
#include <string.h>

//#define DISPLAY_16x2

#define DISPLAY_I2C_ADDRESS 0x27

#ifdef DISPLAY_16x2
#define DISPLAY_SIZE_Y      2
#define DISPLAY_SIZE_X      16
#else
#define DISPLAY_SIZE_Y      4
#define DISPLAY_SIZE_X      20
#endif

bool Display_init(I2C_HandleTypeDef *hi2c){
	lcdInit(hi2c, DISPLAY_I2C_ADDRESS, DISPLAY_SIZE_Y , DISPLAY_SIZE_X);
	return true;
}

bool Display_clear(void){
	lcdDisplayClear();
	return true;
}

bool Display_PrintStr(uint8_t raw, char *data, uint8_t align){
	int len, pos;
	char printStr[DISPLAY_SIZE_X];
	
	if(raw >= DISPLAY_SIZE_Y)
		return false;	
	
	len = strlen(data);
	
	if(align == DISPLAY_ALIGN_LEFT){
		if(len >= DISPLAY_SIZE_X){
			memcpy(printStr, data, DISPLAY_SIZE_X); 
		}
		else{
			memset(printStr, ' ', DISPLAY_SIZE_X);
			memcpy(printStr, data, len);
		}
	}
	else if(align == DISPLAY_ALIGN_RIGHT){
	}
	else{
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
	}
	
	lcdSetCursorPosition(0, raw);
	lcdPrintStr((uint8_t*)printStr, DISPLAY_SIZE_X);
	
	return true;
}

bool Display_PrintStrCenter(uint8_t raw, char *data){
	return Display_PrintStr(raw, data, DISPLAY_ALIGN_CENTER);
}

bool Display_PrintStrLeft(uint8_t raw, char *data){
	return Display_PrintStr(raw, data, DISPLAY_ALIGN_LEFT);
}
