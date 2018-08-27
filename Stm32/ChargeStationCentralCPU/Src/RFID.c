#include "RFID.h"
#include "RC522.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>
#include "tasks.h"

#define CHECK_CONNECTION_SHOT 3

osThreadId pollingTaskHandle;
static QueueHandle_t hQueue;
static uint8_t hTaskTag;

uint32_t lastSelectedCardId = 0;
bool isCardSelected = false;
bool isAdapterConnected = false;


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
	isAdapterConnected = (MFRC522_CheckModule() == MI_OK);
	return isAdapterConnected;
}

void sendConnectionInfoToMain(){
	GeneralMessage message;
	message.sourceTag = hTaskTag;
	message.messageId = MESSAGE_RFID_CONNECTION;
	message.param1 = isAdapterConnected ? 1 : 0;
	xQueueSend(hQueue, &message, 10);
}

void pollingThread(void const * argument){
	uint8_t	cardStr[MFRC522_MAX_LEN];
	uint32_t selCardId;
	uint32_t lastCheckTick, curTick;
	int checkShot;
	GeneralMessage message;
	
	printf("RFID start polling thread\n");
	
	message.sourceTag = hTaskTag;
	lastCheckTick = HAL_GetTick();
	checkShot = CHECK_CONNECTION_SHOT;
	
	for(;;){
		if(isAdapterConnected){
  		if(MFRC522_Check(cardStr) == MI_OK){
	  		memcpy(&selCardId, cardStr, 4);
		  	if(selCardId != lastSelectedCardId){
			  	printf("Found card 0x%.8X\n", selCardId);
				  message.messageId = MESSAGE_RFID_FOUND_CARD;
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
			curTick = HAL_GetTick();
			if((curTick - lastCheckTick) >= 1000){
				//Check if connection with adapter not been lost
				if(MFRC522_CheckModule() != MI_OK){
					if(--checkShot <= 0){
						isAdapterConnected = false;
						sendConnectionInfoToMain();
					}
				}
				lastCheckTick = curTick;
			}
      osDelay(1);
	  }
		else{
			//If not connection check every second for restore
			if(RFID_check_connection()){
				//Connection is restored. Need to inform main thread.
				checkShot = CHECK_CONNECTION_SHOT;
				sendConnectionInfoToMain();
			}
			else{
				osDelay(1000);
			}
		}
  }
}

void RFID_start(uint8_t taskTag, QueueHandle_t queue){
	hTaskTag = taskTag;
	hQueue = queue;
	
	osThreadDef(RFIDTask, pollingThread, osPriorityNormal, 0, 128);
  pollingTaskHandle = osThreadCreate(osThread(RFIDTask), NULL);
}
