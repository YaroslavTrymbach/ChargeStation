#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include <FreeRTOS.h>
#include <queue.h>
#include "ocpp.h"

#define MESSAGE_NET_SERVER_ACCEPT            1
#define MESSAGE_NET_AUTHORIZE                2 
#define MESSAGE_NET_UNLOCK_CONNECTOR         3
#define MESSAGE_NET_REMOTE_START_TRANSACTION 4
#define MESSAGE_NET_REMOTE_STOP_TRANSACTION  5

#define NET_INPUT_MESSAGE_RECONNECT                       1
#define NET_INPUT_MESSAGE_AUTHORIZE                       2
#define NET_INPUT_MESSAGE_SEND_CHARGE_POINT_STATUS        3
#define NET_INPUT_MESSAGE_SEND_CONNECTOR_STATUS           4
#define NET_INPUT_MESSAGE_START_TRANSACTION               5
#define NET_INPUT_MESSAGE_STOP_TRANSACTION                6
#define NET_INPUT_MESSAGE_UNLOCK_CONNECTOR_ANSWER         7
#define NET_INPUT_MESSAGE_REMOTE_START_TRANSACTION_ANSWER 8
#define NET_INPUT_MESSAGE_REMOTE_STOP_TRANSACTION_ANSWER  9

typedef struct _NetInputMessage{
	uint8_t messageId;
	uint32_t param1;
	uint32_t uniqIdIndex;
	idToken tagId;
} NetInputMessage;

void NET_start(uint8_t taskTag, QueueHandle_t queue);

void NET_changeLocalIp(void);

void NET_test(void);

void NET_sendInputMessage(NetInputMessage *message);

bool NET_is_station_accepted(void);
