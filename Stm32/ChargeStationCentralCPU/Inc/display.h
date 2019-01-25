#include <stdbool.h>
#include "stm32f4xx_hal.h"

#define DISPLAY_ALIGN_LEFT   0
#define DISPLAY_ALIGN_CENTER 1
#define DISPLAY_ALIGN_RIGHT  2

bool Display_init(I2C_HandleTypeDef *hi2c);

bool Display_clear(void);

bool Display_PrintStrCenter(uint8_t raw, char *data);

bool Display_PrintStrLeft(uint8_t raw, char *data);
