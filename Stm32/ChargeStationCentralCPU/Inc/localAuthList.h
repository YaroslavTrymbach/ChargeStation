#include "chargePointSettings.h"

#define LOCAL_AUTH_LIST_MAX_LENGTH 256

int localAuthList_getVersion(void);
void localAuthList_setVersion(int newVersion);

void localAuthList_add(AuthorizationData *data);
void localAuthList_clear(void);
int localAuthList_getSize(void);
AuthorizationData* localAuthList_getData(int index);
