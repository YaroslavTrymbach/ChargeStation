#ifndef _WIN32
#include <stdint.h>
#else
#include "defTypes.h"
#endif

uint32_t generateRnd32(void);

void generateGUID(char *GUID);
