#include "channel.h"
#include "cmsis_os.h"
#include "string.h"
#include "stdlib.h"
#include "string_ext.h"
#include "tasks.h"
#include "ocpp.h"

UART_HandleTypeDef *uart;

osThreadId dispatcherTaskHandle;
osThreadId readTaskHandle;

SemaphoreHandle_t hGetStringEvent;
SemaphoreHandle_t hAnswerGotEvent;
ChargePointConnector *requestConnector;
int requestCommand;

static uint8_t hTaskTag;
static QueueHandle_t hMainQueue;

#define COMMAND_GET_STATUS       1
#define COMMAND_START_CHARGING   2
#define COMMAND_HALT_CHARGING    3
#define COMMAND_GET_METER_VALUE  4
#define COMMAND_GET_PS_STATE     5
#define COMMAND_UNLOCK_CONNECTOR 6
#define COMMAND_GET_LOCK_STATE   7

#define FIFO_IN_SIZE 128
unsigned char fifo_in[FIFO_IN_SIZE];
int fifo_in_pos_start = 0;
int fifo_in_pos_end   = 0;
bool fifo_in_is_full = false;
static ChargePointConnector *connector;
static int connectorCount;
uint32_t maxWaitAnswerTick;

void usart_infifo_put(uint8_t c){
	fifo_in[fifo_in_pos_end++] = c;
	
	if(fifo_in_pos_end >= FIFO_IN_SIZE)
		fifo_in_pos_end = 0;
	
	if(fifo_in_is_full){
		fifo_in_pos_start = fifo_in_pos_end;
	}
	else if(fifo_in_pos_end == fifo_in_pos_start){
		fifo_in_is_full = true;
	}		
}

bool usart_fifo_in_is_empty(void){
	return (fifo_in_pos_end == fifo_in_pos_start) && (!fifo_in_is_full);
}

uint8_t usart_fifo_in_pull(void){
	uint8_t c = fifo_in[fifo_in_pos_start++];
	if(fifo_in_pos_start >= FIFO_IN_SIZE)
		fifo_in_pos_start = 0;
	fifo_in_is_full = false;
	return c;
}

int get_string_from_fifo_in(uint8_t *str, uint8_t size){
	uint8_t cnt = 0;
	uint8_t c;
	while(cnt < size){
		if(usart_fifo_in_is_empty())
			break;
		c = usart_fifo_in_pull();	
		if(c == '\r')
			break;
		str[cnt++] = c;			
	}
	str[cnt] = '\0';		
	return cnt;	
}

static void sendMessage(GeneralMessage *message){
	message->sourceTag = hTaskTag;
	xQueueSend(hMainQueue, message, 10);
}

void turnOnInterrupt(){
	__HAL_UART_ENABLE_IT(uart, UART_IT_RXNE);
	HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
}

bool processAnswerGetStatus(ChargePointConnector *conn, char *s, int len){
	int status;
	if(len < 1)
		return false;
	if(!getIntFromHexStr(s, 1, &status))
		return false;
	conn->status = status;
	return true;
}

bool processAnswerGetPilotSygnalState(ChargePointConnector *conn, char *s, int len){
	//Answer must have 2 character 
	// 1-st PilotSygnal Level
	// 2-nd PilotSygnal PWM State
	int level;
	int pwmState;
	int newStatus;
	if(len < 2)
		return false;
	
	//Level
	switch(s[0]){
		case 'A': level = PILOT_SYGNAL_LEVEL_A;
			break;
		case 'B': level = PILOT_SYGNAL_LEVEL_B;
			break;
		case 'C': level = PILOT_SYGNAL_LEVEL_C;
			break;
		case 'D': level = PILOT_SYGNAL_LEVEL_D;
			break;
		case 'E': level = PILOT_SYGNAL_LEVEL_E;
			break;
		default:
			level = PILOT_SYGNAL_LEVEL_F;		
	}
	
	//PWM state
	switch(s[1]){
		case '0': pwmState = PILOT_SYGNAL_PWM_NONE;
			break;
		case '1': pwmState = PILOT_SYGNAL_PWM_7kWt;
			break;
		case '2': pwmState = PILOT_SYGNAL_PWM_22kWt;
			break;
		default:
			pwmState = PILOT_SYGNAL_PWM_UNKNOWN;
	}
	
	if((level != conn->pilotSygnalLevel) || (pwmState != conn->pilotSygnalPwm)){
		newStatus = CHARGE_POINT_STATUS_UNKNOWN;
		
		if(level == PILOT_SYGNAL_LEVEL_A){
			newStatus = CHARGE_POINT_STATUS_AVAILABLE;
		}
		else if(level == PILOT_SYGNAL_LEVEL_B){
			if(conn->status == CHARGE_POINT_STATUS_AVAILABLE)
				newStatus = CHARGE_POINT_STATUS_PREPARING;
			else{
				//It's may be suspended by EV or EVSE
				if(pwmState == PILOT_SYGNAL_PWM_NONE){
					if(conn->status != CHARGE_POINT_STATUS_FINISHING)
						newStatus = CHARGE_POINT_STATUS_SUSPENDED_EVSE;
				}
				else if(pwmState != PILOT_SYGNAL_PWM_UNKNOWN)
					newStatus = CHARGE_POINT_STATUS_SUSPENDED_EV;
			}
		}
		else if((level == PILOT_SYGNAL_LEVEL_C) || (level == PILOT_SYGNAL_LEVEL_D)){
			if(pwmState != PILOT_SYGNAL_PWM_NONE){
				newStatus = CHARGE_POINT_STATUS_CHARGING;
			}
		}

		
		if(newStatus != CHARGE_POINT_STATUS_UNKNOWN){
			conn->status = newStatus;
		}
		
		//Something changed
		conn->pilotSygnalLevel = level;
		conn->pilotSygnalPwm = pwmState;
	}

	return true;
}

bool processAnswerUnlockConnector(ChargePointConnector *conn, char *s, int len, char startChar){
	if(startChar == START_CHAR_SUCCESS)
		conn->isLocked = false;
	else if(startChar == START_CHAR_ERROR)
		conn->isLocked = true;
	else
		return false;
	
	return true;
}


bool processAnswerGetMeterValue(ChargePointConnector *conn, char *s, int len, char startChar){
	int iVal;
	bool isSuccess;
	if(startChar == START_CHAR_SUCCESS){					
		if(getIntFromStr(s, len, &iVal)){
			requestConnector->meterValue = iVal;
			requestConnector->isMeterValueSet = true;
			isSuccess = true;
		}
	}
	else if(startChar == START_CHAR_ERROR){
		if(getIntFromStr(s, len, &iVal)){
			requestConnector->meterValueError = iVal;
			requestConnector->isMeterValueSet = false;
			isSuccess = true;
		}
	}
	
	if(isSuccess){
		requestConnector->isMeterValueRequest = false;
	}
	
	return isSuccess;
}

static void readThread(void const *argument){
	int size;
	int i, address;
	uint8_t getStr[128];
	char *answer;
	char startChar;
	bool isSuccess;
	
	for(;;){
		if(xSemaphoreTake(hGetStringEvent, portMAX_DELAY) == pdPASS){
			size = get_string_from_fifo_in(getStr, 128);
			//It is need to search start position
			answer = NULL;
			for(i = size - 1; i >= 0; i--){
				if((getStr[i] == '!') || (getStr[i] == '?')){
					startChar = getStr[i];
					answer = (char*)(getStr + i);
					size -= i;
					break;
				}
			}
			
			//Answer from channel cpu must contain at least 1 header character
			//and two character of 
			if((answer == NULL) || (size < 3))
				continue;
			
			if(!getIntFromHexStr(answer + 1, 2, &address))
				continue;
			
			if(address == requestConnector->address){
				isSuccess = false;
				answer += 3;
				size -= 3;
				switch(requestCommand){
					case COMMAND_GET_STATUS:
						isSuccess = processAnswerGetStatus(requestConnector, answer, size);
						break;
					case COMMAND_GET_PS_STATE:
						isSuccess = processAnswerGetPilotSygnalState(requestConnector, answer, size);
						break;
					case COMMAND_START_CHARGING:
					case COMMAND_HALT_CHARGING:
						if(startChar == '!')
							isSuccess = true;
						break;
					case COMMAND_GET_METER_VALUE:
						isSuccess = processAnswerGetMeterValue(requestConnector, answer, size, startChar);
						break;
					case COMMAND_UNLOCK_CONNECTOR:
						isSuccess = processAnswerUnlockConnector(requestConnector, answer, size, startChar);
						break;
				}
				
				if(isSuccess)
					xSemaphoreGive(hAnswerGotEvent);
			}
		}
	}
}

bool sendCommandToChannel(ChargePointConnector *conn, int command, char *data){
	char sendStr[16];
	requestCommand = command;
	requestConnector = conn;
	
	switch(requestCommand){
		case COMMAND_GET_STATUS:
			sprintf(sendStr, "$0%dS\r", conn->address);
			break;
		case COMMAND_GET_PS_STATE:
			sprintf(sendStr, "$0%dP\r", conn->address);
			break;
		case COMMAND_GET_METER_VALUE:
			sprintf(sendStr, "$0%dM\r", conn->address);
			break;
		case COMMAND_START_CHARGING:
			sprintf(sendStr, "#0%dS\r", conn->address);
			break;
		case COMMAND_HALT_CHARGING:
			sprintf(sendStr, "#0%dH\r", conn->address);
			break;
		case COMMAND_UNLOCK_CONNECTOR:
			sprintf(sendStr, "#0%dU\r", conn->address);
			break;
		default:
			return false;
	}
	
	HAL_UART_Transmit_DMA(uart, (uint8_t*)sendStr, strlen(sendStr));
	return (xSemaphoreTake(hAnswerGotEvent, maxWaitAnswerTick) == pdPASS);
}

void dispatcherThread(void const * argument){
//#define MAIN_PERIOD 100	
	#define MAIN_PERIOD 1000
	int i;
	uint32_t lastSendTick = 0;
	uint32_t currentTick;
	int prevValue;
	GeneralMessage message;
	ChargePointConnector *conn;
	
	maxWaitAnswerTick = pdMS_TO_TICKS(10);
	
	turnOnInterrupt();
	
	for(;;){
		currentTick = HAL_GetTick();
		if((currentTick - lastSendTick) >= MAIN_PERIOD){
			lastSendTick = currentTick;
			
			//Status request		
			for(i = 0; i < connectorCount; i++){
			
				prevValue = connector[i].status;
				if(sendCommandToChannel(&connector[i], COMMAND_GET_PS_STATE, NULL)){
					connector[i].noAnswerCnt = 0;	
          connector[i].online = true;					
				}
				else{
					if(++(connector[i].noAnswerCnt) >= 5){
						connector[i].status = CHARGE_POINT_STATUS_FAULTED;
						connector[i].pilotSygnalLevel = PILOT_SYGNAL_LEVEL_F;
						connector[i].pilotSygnalPwm = PILOT_SYGNAL_PWM_UNKNOWN;
						connector[i].online = false;
						connector[i].errorCode = 5;
					}
				}	
        if(connector[i].status != prevValue){
						printf("Status changed. A%.2X, newState = %d\n", connector[i].address, connector[i].status);
						message.messageId = MESSAGE_CHANNEL_STATUS_CHANGED;
						message.param1 = i;
						sendMessage(&message);
				}				
			}
		}
		
		for(i = 0; i < connectorCount; i++){
			conn = &connector[i];
			
			if(!conn->online)
				continue;
			
			//Start charging
			if(conn->isNeedStartCharging){
				if(sendCommandToChannel(conn, COMMAND_START_CHARGING, NULL)){
					conn->isNeedStartCharging = false;
				}
			}
			
			//Halt charging
			if(conn->isNeedHaltCharging){
				if(sendCommandToChannel(conn, COMMAND_HALT_CHARGING, NULL)){
					conn->isNeedHaltCharging = false;
					message.messageId = MESSAGE_CHANNEL_CHARGING_HALTED;
					message.param1 = i;
					sendMessage(&message);
				}				
			}
			
			//UnlockConnector
			if(conn->isNeedUnlockConnector){
				if(sendCommandToChannel(conn, COMMAND_UNLOCK_CONNECTOR, NULL)){
					conn->isNeedUnlockConnector = false;
					message.messageId = MESSAGE_CHANNEL_UNLOCK_CONNECTOR;
					message.param1 = conn->isLocked ? UNLOCK_STATUS_UNLOCK_FAILED : UNLOCK_STATUS_UNLOCKED;
					message.param2 = conn->uniqMesIndUnlockConnector;
					sendMessage(&message);
				}
			}

			//Meter value
			if(conn->isMeterValueRequest){
				if(sendCommandToChannel(conn, COMMAND_GET_METER_VALUE, NULL)){
					conn->isMeterValueRequest = false;
					message.messageId = MESSAGE_CHANNEL_GET_METER_VALUE;
					message.param1 = i;
					sendMessage(&message);
				}
			}
		}
	}
}


bool Channel_init(UART_HandleTypeDef *port){
	uart = port;
	return true;
}

bool Channel_start(uint8_t taskTag, QueueHandle_t queue, ChargePointConnector *conn, int count){
	hMainQueue = queue;
	hTaskTag = taskTag;
	connector = conn;
	connectorCount = count;

	hGetStringEvent = xSemaphoreCreateBinary();
	hAnswerGotEvent = xSemaphoreCreateBinary(); 
	
	//Start dispatcher thread
	osThreadDef(dispatcherTask, dispatcherThread, osPriorityNormal, 0, 128);
  dispatcherTaskHandle = osThreadCreate(osThread(dispatcherTask), NULL);
	
	//Start read thread
	osThreadDef(readTask, readThread, osPriorityNormal, 0, 64);
  dispatcherTaskHandle = osThreadCreate(osThread(readTask), NULL);
	return true;
}

void Channel_startCharging(int ch){
	connector[ch].isNeedStartCharging = true;
}

void Channel_haltCharging(int ch){
	connector[ch].isNeedHaltCharging = true;
}

void Channel_unlockConnector(int ch, int uniqMesInd){
	if((ch >= 0) && (ch < connectorCount)){
		connector[ch].isNeedUnlockConnector = true;
		connector[ch].uniqMesIndUnlockConnector = uniqMesInd;
	}
}

void USART2_IRQHandler(void){	
	uint8_t c;
	if(USART2->SR & USART_SR_RXNE){
		c = USART2->DR;
		usart_infifo_put(c);
		if(c == '\r')
			xSemaphoreGiveFromISR(hGetStringEvent, NULL);
	}
	else{
		HAL_UART_IRQHandler(uart);
	}
}
