#include "ocpp.h"

const char* EMPTY_STRING = "\0";

const char* ACTION_STR_BOOT_NOTIFICATION = "BootNotification\0";

const char *getActionString(int action){
	const char* res = EMPTY_STRING;

	switch(action){
		case ACTION_BOOT_NOTIFICATION:
			res = ACTION_STR_BOOT_NOTIFICATION;
			break;
	}

	return res;
}

int getActionFromString(char *s){
}
