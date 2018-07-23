#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include <FreeRTOS.h>
#include <queue.h>

#define MESSAGE_SER_CONTROL_SET_CHARGE_POINT_ID 1
#define MESSAGE_SER_CONTROL_SET_LOCAL_IP        2
#define MESSAGE_SER_CONTROL_SET_SERVER_HOST     3
#define MESSAGE_SER_CONTROL_SET_SERVER_PORT     4
#define MESSAGE_SER_CONTROL_SET_SERVER_URI      5

void SerialControl_start(uint8_t taskTag, QueueHandle_t queue);
