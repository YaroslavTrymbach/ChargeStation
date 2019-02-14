#include "tools.h"
#include "string.h"

bool charToInt(const char c, int *value){
	if((c < 0x30) || (c > 0x39))
		return false;
	*value = c - 0x30;
	return true;
}

bool strToIntWithTrim(const char *s, int *value){
#define STATE_START 0
#define STATE_SIGN	1 
#define STATE_DIGIT 2
#define STATE_END	  3
	int len, state, res, i, d;
  bool isNeg, isFailed;
	char c;
  len	= strlen(s);
	res = 0;
	isNeg = false;
	isFailed = false;
	state = STATE_START;
  for(i = 0; i < len; i++){
		c = s[i];
		switch(state){
			case STATE_START:
				if(charToInt(c, &d)){
					res = d;
					state = STATE_DIGIT;
				}
				else if(c == '-'){
					isNeg = true;
					state = STATE_SIGN;
				}
				else if(c != ' '){
					isFailed = true;
				}
				break;
			case STATE_SIGN:
				//Only digit permitted
			  if(charToInt(c, &d)){
					res = d;
					state = STATE_DIGIT;
				}
				else{
					isFailed = true;
				}
				break;	
			case STATE_DIGIT:
				if(charToInt(c, &d)){
					res *= 10;
					res += d;
				}
				else if(c == ' '){
					state = STATE_END;
				}
				else{
					isFailed = true;
				}
				break;
			case STATE_END:
				//Only spaces is permitted
			  if(c != ' ')
					isFailed = true;
				break;
		}
		if(isFailed)
			break;
	}		
	
	if(isFailed || (state < STATE_DIGIT))
		return false;
	
	if(isNeg)
		res = res * (-1);
	
	*value = res;
	
	return true;
}
