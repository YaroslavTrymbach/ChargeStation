#pragma once

#ifndef _WIN32
#include <stdbool.h>
#else
#include "defTypes.h"
#endif

bool cpState_isServerOnline(void);
void cpState_setServerOnline(bool online);

bool cpState_isNetworkOnline(void);
void cpState_setNetworkOnline(bool online);
