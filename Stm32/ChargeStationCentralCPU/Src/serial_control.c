#include "serial_control.h"
#include "tasks.h"
#include <cmsis_os.h>
#include "usbd_cdc_if.h"
#include "chargePointTime.h"

const char* STR_OK = "OK";

#define GET_BUF_SIZE 256
#define MAX_COMMAND_LENGTH 32

char getBuf[GET_BUF_SIZE];
int getCnt = 0;
char commandStr[MAX_COMMAND_LENGTH];
char params[GET_BUF_SIZE - MAX_COMMAND_LENGTH];

osThreadId serialControlTaskHandle;
QueueHandle_t hMainQueue;
QueueHandle_t hSerialControlGetCharQueue = NULL;

#define sendOK() sendString(STR_OK)

#define ERROR_STR_UNKNOWN_COMMAND    "Unknown command"
#define ERROR_STR_INVALID_PARAMETERS "Invalid parameters"
#define ERROR_STR_BUFFER_OVERFLOW    "Buffer overflow"

#define COMMAND_TYPE_STANDARD 0
#define COMMAND_TYPE_TEST     1   
#define COMMAND_TYPE_READ     2
#define COMMAND_TYPE_WRITE    3 
#define COMMAND_TYPE_EXEC     4

#define COMMAND_UNKNOWN 0

#define COMMAND_EX_TIME 1

void strupr(char *s){
	int i, len;
	len = strlen(s);
	for(i = 0; i < len; i++){
		if((s[i] >= 'a') && (s[i] <= 'z'))
			s[i] -= 0x20;
	}
}

int getCommandFromStr(char *comStr){
	return COMMAND_UNKNOWN;
}

int getCommandExFromStr(char *comStr){
	strupr(comStr);
	if(strcmp(comStr, "TIME") == 0)
		return COMMAND_EX_TIME;
	return COMMAND_UNKNOWN;
}


void sendString(const char *s){
	char sendStr[256];
	sprintf(sendStr, "\r\n%s\r\n", s);
	CDC_Transmit_FS((uint8_t*)sendStr, strlen(sendStr));
}

void sendError(const char *errS){
	char s[64];
	sprintf(s, "ERROR! %s", errS);
	sendString(s);
}

void sendHelloString(){
	char s[64];
	sprintf(s, "Kvant ChargeStation");
	sendString(s);
}

void sendTime(){
	char s[64];
	struct tm t;
	getCurrentTime(&t);
	sprintf(s, "+TIME: %.4d-%.2d-%.2d %.2d:%.2d:%.2d", 
	           t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
	sendString(s);
}

void processCommandExRead(int command){
	switch(command){
		case COMMAND_EX_TIME:
			sendTime();
			break;
	}
	
	sendOK();
}

void processLine(void){
	int commandType = COMMAND_TYPE_STANDARD;
	int commandLen;
	int command;
	char *p;
	printf("Line is get: %s\n", getBuf);
	
	if(getCnt == 0){
		//Just simple send answer OK
		sendOK();
		return;
	}
	
	if(getBuf[0] == '+'){
		// It is extend command
		// Get type of command
		p = strstr(getBuf, "=?");
		if(p != NULL)
			commandType = COMMAND_TYPE_TEST;
		else{
			p = strstr(getBuf, "?");
			if(p != NULL)
				commandType = COMMAND_TYPE_READ;
			else{
				p = strstr(getBuf, "=");
				if(p != NULL)
					commandType = COMMAND_TYPE_WRITE;
				else
					commandType = COMMAND_TYPE_EXEC;
			}			
		}
		
		//Get command
		if(commandType == COMMAND_TYPE_EXEC)
			commandLen = strlen(getBuf);
		else{
			commandLen = (p - getBuf);
		}
		commandLen -= 1; //Start symbol '+' is needless
		
		if(commandLen >= MAX_COMMAND_LENGTH){
			sendError(ERROR_STR_UNKNOWN_COMMAND);
			return;
		}
		
		strncpy(commandStr, getBuf + 1, commandLen);
		commandStr[commandLen] = '\0';
		
		command = getCommandExFromStr(commandStr);
		if(command == COMMAND_UNKNOWN){
			sendError(ERROR_STR_UNKNOWN_COMMAND);
			return;
		}
		
		switch(commandType){
			case COMMAND_TYPE_READ:
				processCommandExRead(command);
				break;
		}
	}
	else{
		//Standard AT command
		/*command = getCommandFromStr(getBuf);
		if(command == COMMAND_UNKNOWN){
			sendError(ERROR_STR_UNKNOWN_COMMAND);
			return;
		}*/
		sendError(ERROR_STR_UNKNOWN_COMMAND);
	}
	
}

#define STATE_WAIT_HEAD1 0  //Wait first symbol from start sequence "AT"
#define STATE_WAIT_HEAD2 1  //Wait first symbol from start sequence "AT"
#define STATE_WAIT_RETURN 2 //Carriage return for end line

void serialControlThread(void const * argument){
	char c;
	int cnt = 0;
	int state = STATE_WAIT_HEAD1;
  //Creating queue for receive 
	hSerialControlGetCharQueue = xQueueCreate(64, sizeof(char));

	sendOK();
	//sendHelloString();

	while(true){
		
		if(xQueueReceive(hSerialControlGetCharQueue, &c, portMAX_DELAY) == pdPASS){
			//printf("SerialControl we got char %c\n", c);
			switch(state){
				case STATE_WAIT_HEAD1:
					if((c == 'a') || (c == 'A')){
						state = STATE_WAIT_HEAD2;
					}
					break;
				case STATE_WAIT_HEAD2:	
					if((c == 't') || (c == 'T')){
						state = STATE_WAIT_RETURN;
					}
					else{
						state = STATE_WAIT_HEAD1;
					}
					break;
				case STATE_WAIT_RETURN:
					if(c == '\r'){
						//End line
						getBuf[getCnt] = '\0';
						processLine();
						state = STATE_WAIT_HEAD1;
						getCnt = 0;						
					}
					else if(c == 0x08){
						//Backspase
						if(getCnt > 0)
							getCnt--;
						else
							state = STATE_WAIT_HEAD2;
					}
					else{
						//Symbol to put in line
						if(getCnt < (GET_BUF_SIZE - 1)){
							getBuf[getCnt++] = c;
						}
						else{
							//Buffer overflow
							sendError(ERROR_STR_BUFFER_OVERFLOW);
							state = STATE_WAIT_HEAD1;
							getCnt = 0;
						}
					}
						
					break;
			}
			//sendOK();
			//HAL_Delay(1000);
		}
		//printf("serialControlThread Cnt = %d\n", ++cnt);
		//vTaskDelay(pdMS_TO_TICKS(1000));
		//HAL_Delay(1000);
	}
}

void SerialControl_start(uint8_t taskTag, QueueHandle_t queue){
	hMainQueue = queue;
  osThreadDef(SerialControlTask, serialControlThread, osPriorityNormal, 0, 256);
  serialControlTaskHandle = osThreadCreate(osThread(SerialControlTask), NULL);
}
