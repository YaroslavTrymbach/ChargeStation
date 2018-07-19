#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include <FreeRTOS.h>
#include <queue.h>

#define MESSAGE_SER_CONTROL_SET_LOCAL_IP 1

void SerialControl_start(uint8_t taskTag, QueueHandle_t queue);
