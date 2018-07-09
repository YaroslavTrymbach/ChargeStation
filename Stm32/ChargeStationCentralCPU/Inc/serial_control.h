#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include <FreeRTOS.h>
#include <queue.h>

void SerialControl_start(uint8_t taskTag, QueueHandle_t queue);
