#include "settings.h"
#include "stm32f4xx_hal.h"
#include <string.h>
#include "flashMap.h"


#define ADDRESS_CPU_UNIQUE_ID 0x1FFF7A10

#define DEF_DHCP_ENABLED true
#define DEF_SERVER_HOST "0.0.0.0"
#define DEF_SERVER_URI "/"
#define DEF_SERVER_PORT 8080

ChargePointSetting workSettings;

const void *flashStartAddress = (void*)FLASH_SECTOR_PTR_SETTINGS;


void setDefValues(void){
	uint32_t *p;
	p = (uint32_t*)ADDRESS_CPU_UNIQUE_ID;
	sprintf(workSettings.ChargePointId, "ChargePoint%.8X", *p);
	workSettings.isDHCPEnabled = DEF_DHCP_ENABLED;
	memset(workSettings.LocalIp, 0x00 , 4);
	memset(workSettings.NetMask, 0x00 , 4);
	memset(workSettings.GetewayIp, 0x00 , 4);
	strcpy(workSettings.serverHost, DEF_SERVER_HOST);
	strcpy(workSettings.serverUri, DEF_SERVER_URI);
	workSettings.serverPort = DEF_SERVER_PORT;
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
	eraseInit.Sector = FLASH_SECTOR_SETTINGS;
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
	
	dstAdr = FLASH_SECTOR_PTR_SETTINGS;
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
