#pragma once

#ifndef _WIN32
#include <stdbool.h>
#else
#include "defTypes.h"
#endif

bool flash_writeSector(int sectorNo, void* data, int dataSize);
void* flash_getSectorAddress(int sectorNo);
