#include "localAuthList.h"
#include "ocpp.h"
#include "string.h"

typedef struct _TLocalAuthList{
	int version;
	int listSize;
	AuthorizationData list[LOCAL_AUTH_LIST_MAX_LENGTH];
}TLocalAuthList;

static TLocalAuthList localAuthList;

int localAuthList_getVersion(void){
	return localAuthList.version;
}

void localAuthList_setVersion(int newVersion){
	localAuthList.version = newVersion;
}

void localAuthList_clear(void){
	localAuthList.version = 0;
	localAuthList.listSize = 0;
}

void localAuthList_add(AuthorizationData *data){
	int index, i;
	//Check if data with such id already exists
	index = -1;
	for(i = 0; i < localAuthList.listSize; i++){
		if(strcmp(localAuthList.list[i].idTag, data->idTag) == 0){
			index = i;
			break;
		}
	}

	if(index == -1){
		//Not founded
		if(localAuthList.listSize < LOCAL_AUTH_LIST_MAX_LENGTH){
			index = localAuthList.listSize++;
		}
		else{
			//Need replace someone
		}
	}

	if(index != -1)
		memcpy(&localAuthList.list[index], data, sizeof(AuthorizationData));
}

int localAuthList_getSize(void){
	return localAuthList.listSize;
}

AuthorizationData* localAuthList_getData(int index){
	if((index >= 0) && (index < localAuthList.listSize)){
		return &localAuthList.list[index];
	}
	else{
		return NULL;
	}
}