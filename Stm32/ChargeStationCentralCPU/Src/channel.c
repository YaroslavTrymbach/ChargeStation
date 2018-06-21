#include "channel.h"
#include "cmsis_os.h"
#include "string.h"

UART_HandleTypeDef *uart;

osThreadId dispatcherTaskHandle;

SemaphoreHandle_t hGetStringEvent;

#define FIFO_IN_SIZE 128
unsigned char fifo_in[FIFO_IN_SIZE];
int fifo_in_pos_start = 0;
int fifo_in_pos_end   = 0;
bool fifo_in_is_full = false;

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
		*str = usart_fifo_in_pull();		
		if(*str == '\r')
			break;
		cnt++;		
	}
	return cnt;	
}

void turnOnInterrupt(){
	__HAL_UART_ENABLE_IT(uart, UART_IT_RXNE);
	HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
}

void dispatcherThread(void const * argument){
	int cnt = 0;
	uint32_t lastSendTick = 0;
	uint32_t currentTick;
	uint32_t maxWaitAnswerTick;
	char sendStr[16];
	
	hGetStringEvent = xSemaphoreCreateBinary();
	maxWaitAnswerTick = pdMS_TO_TICKS(10);
	
	turnOnInterrupt();
	
	for(;;){
		currentTick = HAL_GetTick();
		if((currentTick - lastSendTick) >= 1000){
			lastSendTick = currentTick;
			sprintf(sendStr, "$0%dM\r", (cnt % 2) ? 4 : 5);
			printf("disp send: %d\n", cnt++);
			//HAL_UART_Transmit(uart, (uint8_t*)sendStr, strlen(sendStr), 10);
			HAL_UART_Transmit_DMA(uart, (uint8_t*)sendStr, strlen(sendStr));
			if(xSemaphoreTake(hGetStringEvent, maxWaitAnswerTick) == pdPASS){
				printf("answer is got\n");
			}
			else{
				//Answer is not got
				printf("answer is not got\n");
			}
		}
	}
}


bool Channel_init(UART_HandleTypeDef *port){
	uart = port;
	return true;
}

bool Channel_start(void){
	osThreadDef(dispatcherTask, dispatcherThread, osPriorityNormal, 0, 128);
  dispatcherTaskHandle = osThreadCreate(osThread(dispatcherTask), NULL);
	return true;
}



void USART2_IRQHandler(void){	
	uint8_t c;
	if(USART2->SR & USART_SR_RXNE){
		c = USART2->DR;
		usart_infifo_put(USART2->DR);
		if(c == '\r')
			xSemaphoreGiveFromISR(hGetStringEvent, NULL);
	}
	else{
		HAL_UART_IRQHandler(uart);
	}
}
