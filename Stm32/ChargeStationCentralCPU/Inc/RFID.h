#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include <FreeRTOS.h>
#include <queue.h>

#define MESSAGE_RFID_CONNECTION 1
#define MESSAGE_RFID_FOUND_CARD 2

bool RFID_init(SPI_HandleTypeDef *hspi);

bool RFID_check_connection(void);

void RFID_start(uint8_t taskTag, QueueHandle_t queue);
