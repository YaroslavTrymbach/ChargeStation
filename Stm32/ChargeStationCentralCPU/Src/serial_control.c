#include "serial_control.h"
#include "tasks.h"
#include <cmsis_os.h>
#include "usbd_cdc_if.h"

const char* STR_OK = "OK\r";


osThreadId serialControlTaskHandle;
QueueHandle_t hMainQueue;
QueueHandle_t hSerialControlGetCharQueue = NULL;

#define SendOK() sendString(STR_OK)

void sendString(const char *s){
	CDC_Transmit_FS((uint8_t*)s, strlen(s));
}

void serialControlThread(void const * argument){
  //Creating queue for receive 
	hSerialControlGetCharQueue = xQueueCreate(64, sizeof(char));
	while(true){
		SendOK();
		HAL_Delay(1000);
	}
}

void SerialControl_start(uint8_t taskTag, QueueHandle_t queue){
	hMainQueue = queue;
  osThreadDef(SerialControlTask, serialControlThread, osPriorityNormal, 0, 128);
  serialControlTaskHandle = osThreadCreate(osThread(SerialControlTask), NULL);
}
