#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include <FreeRTOS.h>
#include <queue.h>
#include "connector.h"

#define MESSAGE_CHANNEL_STATUS_CHANGED 1

bool Channel_init(UART_HandleTypeDef *port);
bool Channel_start(uint8_t taskTag, QueueHandle_t queue, ChargePointConnector *conn, int count);

void Channel_startCharging(int ch);
void Channel_haltCharging(int ch);
