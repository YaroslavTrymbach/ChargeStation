#include "chargePointSettings.h"

#define LOCAL_AUTH_LIST_MAX_LENGTH 256

void localAuthList_load(void);
bool localAuthList_save(void);

int localAuthList_getVersion(void);
void localAuthList_setVersion(int newVersion);

void localAuthList_add(AuthorizationData *data);
bool localAuthList_deleteByIndex(int index);
bool localAuthList_deleteByTag(idToken tag);
void localAuthList_clear(void);
int localAuthList_getSize(void);
AuthorizationData* localAuthList_getData(int index);
AuthorizationData* localAuthList_getDataByTag(idToken tag);
