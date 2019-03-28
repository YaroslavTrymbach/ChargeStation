#include "localAuthList.h"
#include "ocpp.h"
#include "string.h"
#include "stdint.h"
#include "flash.h"
#include "flashMap.h"

typedef struct _TLocalAuthList{
	int version;
	int listSize;
	AuthorizationData list[LOCAL_AUTH_LIST_MAX_LENGTH];
}TLocalAuthList;

static TLocalAuthList localAuthList;

int findIndexByTag(idToken tag){
	int i;
	for(i = 0; i < localAuthList.listSize; i++){
		if(strcmp(localAuthList.list[i].idTag, tag) == 0){
			return i;
		}
	}
	return -1;
}

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
	int index;
	//Check if data with such id already exists
	index = findIndexByTag(data->idTag);

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

AuthorizationData* localAuthList_getDataByTag(idToken tag){
	int index;
	index = findIndexByTag(tag);
	if(index >= 0){
		return &localAuthList.list[index];
	}
	else{
		return NULL;
	}
}

void localAuthList_load(void){	
	uint32_t *p;
	p = (uint32_t*)flash_getSectorAddress(FLASH_SECTOR_LOCALLIST);
	if(*p == 0xFFFFFFFF){
		//If setting is not set yet. Fill it with default values
		localAuthList_clear();
		return;
	}
	
	//Copy setting from flash
	memcpy(&localAuthList, p, sizeof(localAuthList));
}

bool localAuthList_save(void){
	return flash_writeSector(FLASH_SECTOR_LOCALLIST, (void*)&localAuthList, sizeof(localAuthList));
}

bool localAuthList_deleteByIndex(int index){
	int i;
	//Check if index is correct
	if((index < 0) || (index >= localAuthList.listSize)){
		return false;
	}
	
	//Make deletion
	for(i = index; i < localAuthList.listSize  - 1; i++){
		memcpy(&localAuthList.list[i], &localAuthList.list[i+1], sizeof(AuthorizationData));
	}
	localAuthList.listSize--;
	
	return true;
}

bool localAuthList_deleteByTag(idToken tag){
	return localAuthList_deleteByIndex(findIndexByTag(tag));
}
