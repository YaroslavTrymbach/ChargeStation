#include "RFID.h"
#include "RC522.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>
#include "tasks.h"

osThreadId pollingTaskHandle;
static QueueHandle_t hQueue;
static uint8_t hTaskTag;

uint32_t lastSelectedCardId = 0;
bool isCardSelected = false;


bool RFID_init(SPI_HandleTypeDef *hspi){
	RC522InitType rc522Init;
	rc522Init.hspi = hspi;
	rc522Init.isHardwareChipSelect = false;
	rc522Init.chipSelectPort = GPIOE;
	rc522Init.chipSelectPin = GPIO_PIN_4;
	
	MFRC522_Init(&rc522Init);
	
	return true;
}

bool RFID_check_connection(void){
	return (MFRC522_CheckModule() == MI_OK);
}

void pollingThread(void const * argument){
	uint8_t	cardStr[MFRC522_MAX_LEN];
	uint32_t selCardId;
	GeneralMessage message;
	
	printf("RFID start polling thread\n");
	
	message.sourceTag = hTaskTag;
	
	for(;;){
		if(MFRC522_Check(cardStr) == MI_OK){
			memcpy(&selCardId, cardStr, 4);
			if(selCardId != lastSelectedCardId){
				printf("Found card 0x%.8X\n", selCardId);
				message.messageId = MESSAGE_FOUND_CARD;
				message.param1 = selCardId;
				if(xQueueSend(hQueue, &message, 10) == pdPASS){
					lastSelectedCardId = selCardId;
				}
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

void RFID_start(uint8_t taskTag, QueueHandle_t queue){
	hTaskTag = taskTag;
	hQueue = queue;
	
	osThreadDef(RFIDTask, pollingThread, osPriorityNormal, 0, 128);
  pollingTaskHandle = osThreadCreate(osThread(RFIDTask), NULL);
}
