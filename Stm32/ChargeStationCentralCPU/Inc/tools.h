#pragma once

#ifndef _WIN32
#include <stdbool.h>
#else
#include "defTypes.h"
#endif

bool strToIntWithTrim(const char *s, int *value);
