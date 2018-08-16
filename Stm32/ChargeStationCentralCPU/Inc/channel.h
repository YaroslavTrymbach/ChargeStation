#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "connector.h"

bool Channel_init(UART_HandleTypeDef *port);
bool Channel_start(ChargePointConnector *conn, int count);
