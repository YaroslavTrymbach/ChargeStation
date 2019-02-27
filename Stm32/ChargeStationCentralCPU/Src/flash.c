#include "flash.h"
#include "stm32f4xx_hal.h"

uint32_t sectorStartAddress(int sectorNo){
	switch(sectorNo){
		case 0:
			return 0x08000000;
		case 12:
			return 0x08100000;
		case 13:
			return 0x08104000;
		case 14:
			return 0x08108000;
	}
	return 0;
}

bool flash_writeSector(int sectorNo, void* data, int dataSize){
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
	eraseInit.Sector = sectorNo;
	eraseInit.NbSectors = 1;
	eraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	
	if(HAL_FLASHEx_Erase(&eraseInit, &error) != HAL_OK){
		HAL_FLASH_Lock();
		return false;
	}
	
	//Programming
	writeSize = dataSize;
	wordNumber = writeSize >> 2;
	if(writeSize % 4)
		wordNumber++;
	
	dstAdr = sectorStartAddress(sectorNo);
	pSrc = (uint32_t*)data;
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

void* flash_getSectorAddress(int sectorNo){
	return (void*)sectorStartAddress(sectorNo);
}

