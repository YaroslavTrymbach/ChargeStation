#include "RFID.h"
#include "RC522.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>

osThreadId pollingTaskHandle;

uint32_t lastSelectedCardId = 0;
bool isCardSelected = false;

bool RFID_init(SPI_HandleTypeDef *hspi){
	RC522InitType rc522Init;
	rc522Init.hspi = hspi;
	rc522Init.isHardwareChipSelect = true;
	//rc522Init.chipSelectPort = SPI_CS_GPIO_Port;
	//rc522Init.chipSelectPin = SPI_CS_Pin;
	
	MFRC522_Init(&rc522Init);
	
	return true;
}

bool RFID_check_connection(void){
	return (MFRC522_CheckModule() == MI_OK);
}

void pollingThread(void const * argument){
	uint8_t	cardStr[MFRC522_MAX_LEN];
	uint32_t selCardId;
	printf("RFID start polling thread\n");
	for(;;){
		if(MFRC522_Check(cardStr) == MI_OK){
			memcpy(&selCardId, cardStr, 4);
			if(selCardId != lastSelectedCardId){
				printf("Found card 0x%.8X\n", selCardId);
				lastSelectedCardId = selCardId;
			}
			isCardSelected = true;
		}
		else{
			if(isCardSelected){
				printf("Card is removed\n");
				lastSelectedCardId = 0;				
			}
			isCardSelected = false;
		}
    osDelay(1);
  }
}

void RFID_start(void){
	osThreadDef(RFIDTask, pollingThread, osPriorityNormal, 0, 128);
  pollingTaskHandle = osThreadCreate(osThread(RFIDTask), NULL);
}
