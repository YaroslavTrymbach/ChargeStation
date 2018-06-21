#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include <FreeRTOS.h>
#include <queue.h>

#define MESSAGE_FOUND_CARD 1

bool RFID_init(SPI_HandleTypeDef *hspi);

bool RFID_check_connection(void);

void RFID_start(uint8_t taskTag, QueueHandle_t queue);
