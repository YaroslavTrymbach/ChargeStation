#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include <FreeRTOS.h>
#include <queue.h>

#define MESSAGE_NET_SERVER_ACCEPT   1
#define MESSAGE_NET_AUTHORIZE       2 

#define NET_INPUT_MESSAGE_RECONNECT      1
#define NET_INPUT_MESSAGE_AUTHORIZE      2
#define NET_INPUT_MESSAGE_SEND_STATUS 3

typedef struct _NetInputMessage{
	uint8_t messageId;
	uint32_t param1;
} NetInputMessage;

void NET_start(uint8_t taskTag, QueueHandle_t queue);

void NET_changeLocalIp(void);

void NET_test(void);

void NET_sendInputMessage(NetInputMessage *message);

bool NET_is_station_accepted();
