#include "stm32f4xx_hal.h"
#include <FreeRTOS.h>
#include <queue.h>

#define NET_INPUT_MESSAGE_RECONNECT 1

typedef struct _NetInputMessage{
	uint8_t messageId;
} NetInputMessage;

void NET_start(uint8_t taskTag, QueueHandle_t queue);

void NET_changeLocalIp(void);

void NET_test(void);

void NET_sendInputMessage(NetInputMessage *message);
