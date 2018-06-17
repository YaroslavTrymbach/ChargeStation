#include <stdbool.h>
#include "stm32f4xx_hal.h"

bool RFID_init(SPI_HandleTypeDef *hspi);

bool RFID_check_connection(void);

void RFID_start(void);
