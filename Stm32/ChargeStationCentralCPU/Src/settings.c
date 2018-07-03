#include "settings.h"
#include "stm32f4xx_hal.h"
#include <string.h>

#define FLASH_SECTOR_NUMBER 12 //Номер сектора в котором хранятся настройки
#define FLASH_SECTOR_PTR 0x08100000 //Начальный адресс сектора в котором хранятся настройки

#define ADDRESS_CPU_UNIQUE_ID 0x1FFF7A10

ChargePointSetting workSettings;

const void *flashStartAddress = (void*)FLASH_SECTOR_PTR;


void setDefValues(void){
	uint32_t *p;
	p = (uint32_t*)ADDRESS_CPU_UNIQUE_ID;
	sprintf(workSettings.ChargePointId, "ChargePoint%.8X", *p);
}

void Settings_init(void){
	uint32_t *p;
	p = (uint32_t*)flashStartAddress;
	if(*p == 0xFFFFFFFF){
		//If setting is not set yet. Fill it with default values
		setDefValues();
		return;
	}
	
	//Copy setting from flash
	memcpy(&workSettings, flashStartAddress, sizeof(ChargePointSetting));
	
	//Make some checking
	workSettings.ChargePointId[SIZE_CHARGE_POINT_ID-1] = '\0';
}

ChargePointSetting* Settings_get(void){
	return &workSettings;
}

bool Settings_save(void){
	FLASH_EraseInitTypeDef eraseInit;
	uint32_t error;
	uint32_t dstAdr;
	uint32_t wordNumber, writeSize;
	uint32_t *pSrc;
	int i;
	
	//Unlock flash
	if(HAL_FLASH_Unlock() != HAL_OK)
		return false;
	
	//Erasing sector
	eraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
	eraseInit.Sector = FLASH_SECTOR_NUMBER;
	eraseInit.NbSectors = 1;
	eraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	
	if(HAL_FLASHEx_Erase(&eraseInit, &error) != HAL_OK){
		HAL_FLASH_Lock();
		return false;
	}
	
	//Programming
	writeSize = sizeof(ChargePointSetting);
	wordNumber = writeSize >> 2;
	if(writeSize % 4)
		wordNumber++;
	
	dstAdr = FLASH_SECTOR_PTR;
	pSrc = (uint32_t*)&workSettings;
	for(i = 0; i < wordNumber; i++){
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, dstAdr, *pSrc) != HAL_OK){
			HAL_FLASH_Lock();
			return false;
		}
		dstAdr += 4;
		pSrc += 1;
	}
	
	HAL_FLASH_Lock();
	return true;
}
