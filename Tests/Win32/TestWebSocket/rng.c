#include "rng.h"
#include <stdlib.h>
#include <string.h>

uint32_t generateRnd32(void){
	uint32_t value = (rand() << 16) | rand();
	return value;
}

void generateGUID(char *GUID){
	strncpy(GUID, "c817f3b0-c87a-4ec5-a0a4-721f5b884abe", 36);
}