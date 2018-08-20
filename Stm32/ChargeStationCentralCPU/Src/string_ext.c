#include "string_ext.h"
#include "string.h"

void strupr(char *s){
	int i, len;
	len = strlen(s);
	for(i = 0; i < len; i++){
		if((s[i] >= 'a') && (s[i] <= 'z'))
			s[i] -= 0x20;
	}
}

void strupr_s(char *s, int length){
	int i, len;
	
	len = strlen(s);
	if(len > length)
		len = length;
	
	for(i = 0; i < len; i++){
		if((s[i] >= 'a') && (s[i] <= 'z'))
			s[i] -= 0x20;
	}
}

bool getIntFromHexStr(char *s, int cnt, int *outValue){
	int i, value, digit;
	char c;
	strupr_s(s, cnt);
	value = 0;
	for(i = 0; i < cnt; i++){
		c = s[i];
		if((c >= '0') && (c <= '9'))
			digit = c - '0';
		else if((c >= 'A') && (c <= 'F'))
			digit = c - 'A' + 0x0A;
		else
			return false;
		value = value * 0x10 + digit;
	}
	*outValue = value;
	return true;
}
