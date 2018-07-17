#ifndef _WIN32
#include <stdbool.h>
#else
#include "defTypes.h"
#endif

#include "ocpp.h"

bool jsonPackReqBootNotification(struct RequestBootNotificatation *req, char* outBuf, int* outLen);
