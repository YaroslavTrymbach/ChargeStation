#pragma once

#ifndef _WIN32
#include <stdbool.h>
#else
#include "defTypes.h"
#endif

#include <time.h>



bool getCurrentTime(struct tm* time);
bool setCurrentTime(struct tm* time);