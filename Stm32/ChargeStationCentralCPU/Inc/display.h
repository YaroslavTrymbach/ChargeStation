#include <stdbool.h>
#include "stm32f4xx_hal.h"

bool Display_init(I2C_HandleTypeDef *hi2c);

bool Display_clear(void);

bool Display_PrintStrCenter(uint8_t raw, char *data);
