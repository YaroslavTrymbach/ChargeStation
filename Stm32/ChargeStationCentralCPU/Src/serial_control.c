#include "serial_control.h"
#include "tasks.h"
#include <cmsis_os.h>
#include "usbd_cdc_if.h"
#include "chargePointTime.h"
#include "settings.h"
#include "net.h"
#include "lwip/netif.h"
#include "string_ext.h"
#include "tools.h"

const char* STR_OK = "\r\nOK\r\n";
const char* STR_NEW_LINE_OK = "\r\nOK";

#define GET_BUF_SIZE 256
#define MAX_COMMAND_LENGTH 32

#define getTickCount() pdMS_TO_TICKS(xTaskGetTickCount())

char getBuf[GET_BUF_SIZE];
int getCnt = 0;
char commandStr[MAX_COMMAND_LENGTH];
char params[GET_BUF_SIZE - MAX_COMMAND_LENGTH];

osThreadId serialControlTaskHandle;
static QueueHandle_t hMainQueue;
QueueHandle_t hSerialControlGetCharQueue = NULL;
static uint8_t hTaskTag;

#define sendOK() sendString(STR_OK)

#define ERROR_STR_UNKNOWN_COMMAND    "Unknown command"
#define ERROR_STR_INVALID_PARAMETERS "Invalid parameters"
#define ERROR_STR_BUFFER_OVERFLOW    "Buffer overflow"
#define ERROR_STR_EXEC_FAILED        "Execution if failed"

#define COMMAND_TYPE_STANDARD 0
#define COMMAND_TYPE_TEST     1   
#define COMMAND_TYPE_READ     2
#define COMMAND_TYPE_WRITE    3 
#define COMMAND_TYPE_EXEC     4

#define COMMAND_UNKNOWN 0

#define COMMAND_EX_TIME            1
#define COMMAND_EX_LOCAL_IP        2
#define COMMAND_EX_SAVE_SETTINGS   3
#define COMMAND_EX_SERVER_HOST     4
#define COMMAND_EX_SERVER_PORT     5
#define COMMAND_EX_SERVER_URI      6
#define COMMAND_EX_CHARGE_POINT_ID 7
#define COMMAND_EX_RECONNECT       8

#define DELIM_COMMA ','
#define DELIM_POINT '.'

const char* STR_EMPTY = "";
const char* COMMAND_STR_EX_TIME            = "TIME\0";
const char* COMMAND_STR_EX_LOCAL_IP        = "LOCALIP\0";
const char* COMMAND_STR_EX_SAVE_SETTINGS   = "SAVESETTINGS\0";
const char* COMMAND_STR_EX_SERVER_HOST     = "SERVERHOST\0";
const char* COMMAND_STR_EX_SERVER_PORT     = "SERVERPORT\0";
const char* COMMAND_STR_EX_SERVER_URI      = "SERVERURI\0";
const char* COMMAND_STR_EX_CHARGE_POINT_ID = "CHARGEPOINTID\0";
const char* COMMAND_STR_EX_RECONNECT       = "RECONNECT\0";

extern struct netif gnetif;

void sendMessage(int mesId);
/*
void strupr(char *s){
	int i, len;
	len = strlen(s);
	for(i = 0; i < len; i++){
		if((s[i] >= 'a') && (s[i] <= 'z'))
			s[i] -= 0x20;
	}
}*/
/*
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
}*/

int getCommandFromStr(char *comStr){
	return COMMAND_UNKNOWN;
}


int getCommandExFromStr(char *comStr){
#define CHECK_COMMAND(name) if(strcmp(comStr, COMMAND_STR_EX_##name) == 0) \
		                          return COMMAND_EX_##name 
				
	strupr(comStr);
															
	CHECK_COMMAND(TIME);
	CHECK_COMMAND(LOCAL_IP);
	CHECK_COMMAND(SAVE_SETTINGS);
	CHECK_COMMAND(SERVER_HOST);
	CHECK_COMMAND(SERVER_PORT);
	CHECK_COMMAND(SERVER_URI);
	CHECK_COMMAND(CHARGE_POINT_ID);
	CHECK_COMMAND(RECONNECT);
	
	return COMMAND_UNKNOWN;
}

void sendString(const char *s){
#define CHUNK_MAX_SIZE 60	
	int len, cnt, chunk;
	uint32_t startTick;
	
	//such as COM-port is virtual with size of package 64 bytes
	//it is need to split send string on chunks
	
	startTick = getTickCount();
	len = strlen(s); 
	cnt = 0;
	while(cnt < len){
		chunk = len - cnt;
		if(chunk > CHUNK_MAX_SIZE)
			chunk = CHUNK_MAX_SIZE;
		if(CDC_IsTxFree()){
			CDC_Transmit_FS((uint8_t*)(s + cnt), chunk);		
			cnt += chunk;
		}
		else{
			if((getTickCount() - startTick) >= 5){
				break;
			}
		}
	}
}

#define CASE_COMMAND_EX_STR(name) case COMMAND_EX_##name: \
	                                  comStr = COMMAND_STR_EX_##name; \
								                    break

void sendReadResultString(int command, const char *s){
	char sendStr[256];
	const  char *comStr = STR_EMPTY;
	switch(command){
		CASE_COMMAND_EX_STR(TIME);
		CASE_COMMAND_EX_STR(LOCAL_IP);
		CASE_COMMAND_EX_STR(SERVER_HOST);
		CASE_COMMAND_EX_STR(SERVER_PORT);
		CASE_COMMAND_EX_STR(SERVER_URI);
	}
	sprintf(sendStr, "\r\n+%s: %s\r\nOK\r\n", comStr, s);
	sendString(sendStr);
}

void sendString2(const char *s){
	char sendStr[256];
	sprintf(sendStr, "\r\n%s\r\n", s);
	CDC_Transmit_FS((uint8_t*)sendStr, strlen(sendStr));
}

void sendError(const char *errS){
	char s[64];
	sprintf(s, "\r\nERROR! %s\r\n", errS);
	sendString(s);
}

void sendHelloString(){
	char s[64];
	sprintf(s, "Kvant ChargeStation");
	sendString(s);
}

void sendTime(){
	char s[64];
	struct tm t;
	getCurrentTime(&t);
	sprintf(s, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d", 
	           t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
	sendReadResultString(COMMAND_EX_TIME, s);
	/*sprintf(s, "+TIME: %.4d-%.2d-%.2d %.2d:%.2d:%.2d", 
	           t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
	strcat(s, STR_NEW_LINE_OK);
	sendString(s);*/
}

void sendLocalIp(){
	char s[64];
	unsigned char ip[3][4];
	ChargePointSetting* st;
	st = Settings_get();
	if(st->isDHCPEnabled){
		memcpy(ip[0], &gnetif.ip_addr, 4);
		memcpy(ip[1], &gnetif.netmask, 4);
		memcpy(ip[2], &gnetif.gw, 4);
		sprintf(s, "1, %d.%d.%d.%d, %d.%d.%d.%d, %d.%d.%d.%d", 
		  ip[0][0], ip[0][1], ip[0][2], ip[0][3],
		  ip[1][0], ip[1][1], ip[1][2], ip[1][3],
		  ip[2][0], ip[2][1], ip[2][2], ip[2][3]);
	}
	else{
		sprintf(s, "0, %d.%d.%d.%d, %d.%d.%d.%d, %d.%d.%d.%d", 
		  st->LocalIp[0], st->LocalIp[1], st->LocalIp[2], st->LocalIp[3],
		  st->NetMask[0], st->NetMask[1], st->NetMask[2], st->NetMask[3],
		  st->GetewayIp[0], st->GetewayIp[1], st->GetewayIp[2], st->GetewayIp[3]);
	}
	sendReadResultString(COMMAND_EX_LOCAL_IP, s);
}

char* getParam(char **spar, char chPDiv){
	char *pdest;
	char *sres;
	
	sres = *spar;
	pdest = strchr(sres, chPDiv);
	if( pdest != NULL )
	{
		*pdest= '\0';
		*spar = pdest + 1;
	} 
	else 
		*spar += strlen(sres);

	return sres;
}

bool getParamInt(char **spar, char chPDiv, int *value){
	char *s;
	s = getParam(spar, chPDiv);
	if(s == NULL)
		return false;
	
	return strToIntWithTrim(s, value);
}

bool getParamIp(char **spar, char chPDiv, unsigned char *value){
	char *s;
	int i, iVal;
	s = getParam(spar, chPDiv);
	if(s == NULL)
		return false;
	
	for(i = 0; i < 4; i++){
		if(!getParamInt(&s, DELIM_POINT, &iVal))
			return false;
		if((iVal < 0) || (iVal > 255))
			return false;
		value[i] = iVal;
	}
	return true;
}

void sendMessage(int mesId){
	GeneralMessage message;
	
	//Send message to central process
	message.sourceTag = hTaskTag;
	message.messageId = mesId;
	xQueueSend(hMainQueue, &message, 10);
}

void writeLocalIp(char *arg){
	int iVal, i;
	bool dhcpEnabled;
	unsigned char ip[3][4];
	ChargePointSetting* st;
	
	
	// Waiting string
  // 0, XXX.XXX.XXX.XXX, XXX.XXX.XXX.XXX, XXX.XXX.XXX.XXX
	
	//Get dhcp server enabling
	if(!getParamInt(&arg, DELIM_COMMA, &iVal)){
		sendError(ERROR_STR_INVALID_PARAMETERS);
		return;
	}
	
	//printf("iVal = %d\n", iVal);
	
	if(iVal == 0){
		dhcpEnabled = false;
	}
	else if(iVal == 1){
		dhcpEnabled = true;
	}
	else{
		sendError(ERROR_STR_INVALID_PARAMETERS);
		return;
	}
	
	if(!dhcpEnabled){
		//Ip, Netmask and Gateway is need
		for(i = 0; i < 3; i++){
			if(!getParamIp(&arg, DELIM_COMMA, ip[i])){
				sendError(ERROR_STR_INVALID_PARAMETERS);
				return;
			}				
		}
	}
	
	sendOK();
	
	st = Settings_get();
	st->isDHCPEnabled = dhcpEnabled;
	if(!st->isDHCPEnabled){
		memcpy(st->LocalIp, ip[0], 4);
		memcpy(st->NetMask, ip[1], 4);
		memcpy(st->GetewayIp, ip[2], 4);
	}

	sendMessage(MESSAGE_SER_CONTROL_SET_LOCAL_IP);
}

void writeServerHost(char *arg){
	ChargePointSetting* st;
	
	if(strlen(arg) >= SIZE_SERVER_HOST){
		sendError(ERROR_STR_INVALID_PARAMETERS);
		return;
	}
	
	sendOK();
	
	st = Settings_get();
	strcpy(st->serverHost, arg);
	
	sendMessage(MESSAGE_SER_CONTROL_SET_SERVER_HOST);
}

void writeServerPort(char *arg){
	ChargePointSetting* st;
	int iVal;
	
	if(!getParamInt(&arg, DELIM_COMMA, &iVal)){
		sendError(ERROR_STR_INVALID_PARAMETERS);
		return;
	}
	
	if((iVal < 0) || (iVal > 65535)){
		sendError(ERROR_STR_INVALID_PARAMETERS);
		return;
	}
	
	sendOK();
	
	st = Settings_get();
	st->serverPort = iVal;
	
	sendMessage(MESSAGE_SER_CONTROL_SET_SERVER_PORT);
}

void writeServerURI(char *arg){
	ChargePointSetting* st;
	
	if(strlen(arg) >= SIZE_SERVER_URI){
		sendError(ERROR_STR_INVALID_PARAMETERS);
		return;
	}
	
	sendOK();
	
	st = Settings_get();
	strcpy(st->serverUri, arg);
	
	sendMessage(MESSAGE_SER_CONTROL_SET_SERVER_URI);
}

void writeChargePointID(char *arg){
	ChargePointSetting* st;
	
	if(strlen(arg) >= SIZE_CHARGE_POINT_ID){
		sendError(ERROR_STR_INVALID_PARAMETERS);
		return;
	}
	
	sendOK();
	
	st = Settings_get();
	strcpy(st->ChargePointId, arg);
	
	sendMessage(MESSAGE_SER_CONTROL_SET_CHARGE_POINT_ID);
}

void execSaveSettings(){
	if(Settings_save()){
		sendOK();
	}
	else{
		sendError(ERROR_STR_EXEC_FAILED);
	}
}

void execReconnect(){
	NetInputMessage mes;
	mes.messageId = NET_INPUT_MESSAGE_RECONNECT;
	NET_sendInputMessage(&mes);
	sendOK();
}

void processCommandExWrite(int command, char *arg){
	switch(command){
		case COMMAND_EX_LOCAL_IP:
			writeLocalIp(arg);
			break;
		case COMMAND_EX_SERVER_HOST:
			writeServerHost(arg);
			break;
		case COMMAND_EX_SERVER_PORT:
			writeServerPort(arg);
			break;
		case COMMAND_EX_SERVER_URI:
			writeServerURI(arg);
			break;
		case COMMAND_EX_CHARGE_POINT_ID:
			writeChargePointID(arg);
			break;
	}
}

void processCommandExRead(int command){
	ChargePointSetting* st;
	char *answerStr;
	char s[16];
	
	switch(command){
		case COMMAND_EX_TIME:
			sendTime();
		  return;
		case COMMAND_EX_LOCAL_IP:
			sendLocalIp();
		  return;
		case COMMAND_EX_SERVER_HOST:
			st = Settings_get();
		  answerStr = st->serverHost;
			break;
		case COMMAND_EX_SERVER_URI:
			st = Settings_get();
			answerStr = st->serverUri;
			break;
		case COMMAND_EX_SERVER_PORT:
			st = Settings_get();
		  sprintf(s, "%d", st->serverPort);
		  answerStr = s;
			break;
		case COMMAND_EX_CHARGE_POINT_ID:
			st = Settings_get();
			answerStr = st->ChargePointId;
			break;
		
		default:
			return;
	}
	
	sendReadResultString(command, answerStr);
}

void processCommandExExec(int command){
	switch(command){
		case COMMAND_EX_SAVE_SETTINGS:
			execSaveSettings();
			break;
		case COMMAND_EX_RECONNECT:
			execReconnect();
			break;
	}
}

void processLine(void){
#define ARG_SIZE 128	
	int commandType = COMMAND_TYPE_STANDARD;
	int commandLen;
	int command;
	char *p;
	char *arg;
	printf("Line is get: %s\n", getBuf);
	
	if(getCnt == 0){
		//Just simple send answer OK
		sendOK();
		return;
	}
	
	if(getBuf[0] == '+'){
		// It is extend command
		// Get type of command
		p = strstr(getBuf, "=?");
		if(p != NULL)
			commandType = COMMAND_TYPE_TEST;
		else{
			p = strstr(getBuf, "?");
			if(p != NULL)
				commandType = COMMAND_TYPE_READ;
			else{
				p = strstr(getBuf, "=");
				if(p != NULL)
					commandType = COMMAND_TYPE_WRITE;
				else
					commandType = COMMAND_TYPE_EXEC;
			}			
		}
		
		//Get command
		if(commandType == COMMAND_TYPE_EXEC)
			commandLen = strlen(getBuf);
		else{
			commandLen = (p - getBuf);
		}
		commandLen -= 1; //Start symbol '+' is needless
		
		if(commandLen >= MAX_COMMAND_LENGTH){
			sendError(ERROR_STR_UNKNOWN_COMMAND);
			return;
		}
		
		strncpy(commandStr, getBuf + 1, commandLen);
		commandStr[commandLen] = '\0';
		
		command = getCommandExFromStr(commandStr);
		if(command == COMMAND_UNKNOWN){
			sendError(ERROR_STR_UNKNOWN_COMMAND);
			return;
		}
		
		switch(commandType){
			case COMMAND_TYPE_READ:
				processCommandExRead(command);
				break;
			case COMMAND_TYPE_WRITE:
				arg = p + 1; 
				processCommandExWrite(command, arg);
				break;
			case COMMAND_TYPE_EXEC:
				processCommandExExec(command);
				break;
		}
	}
	else{
		//Standard AT command
		/*command = getCommandFromStr(getBuf);
		if(command == COMMAND_UNKNOWN){
			sendError(ERROR_STR_UNKNOWN_COMMAND);
			return;
		}*/
		sendError(ERROR_STR_UNKNOWN_COMMAND);
	}
	
}

#define STATE_WAIT_HEAD1 0  //Wait first symbol from start sequence "AT"
#define STATE_WAIT_HEAD2 1  //Wait first symbol from start sequence "AT"
#define STATE_WAIT_RETURN 2 //Carriage return for end line

void serialControlThread(void const * argument){
	char c;
	int cnt = 0;
	int state = STATE_WAIT_HEAD1;
  //Creating queue for receive 
	hSerialControlGetCharQueue = xQueueCreate(64, sizeof(char));

	sendOK();
	//sendHelloString();

	while(true){
		
		if(xQueueReceive(hSerialControlGetCharQueue, &c, portMAX_DELAY) == pdPASS){
			//printf("SerialControl we got char %c\n", c);
			switch(state){
				case STATE_WAIT_HEAD1:
					if((c == 'a') || (c == 'A')){
						state = STATE_WAIT_HEAD2;
					}
					break;
				case STATE_WAIT_HEAD2:	
					if((c == 't') || (c == 'T')){
						state = STATE_WAIT_RETURN;
					}
					else{
						state = STATE_WAIT_HEAD1;
					}
					break;
				case STATE_WAIT_RETURN:
					if(c == '\r'){
						//End line
						getBuf[getCnt] = '\0';
						processLine();
						state = STATE_WAIT_HEAD1;
						getCnt = 0;						
					}
					else if(c == 0x08){
						//Backspase
						if(getCnt > 0)
							getCnt--;
						else
							state = STATE_WAIT_HEAD2;
					}
					else{
						//Symbol to put in line
						if(getCnt < (GET_BUF_SIZE - 1)){
							getBuf[getCnt++] = c;
						}
						else{
							//Buffer overflow
							sendError(ERROR_STR_BUFFER_OVERFLOW);
							state = STATE_WAIT_HEAD1;
							getCnt = 0;
						}
					}
						
					break;
			}
			//sendOK();
			//HAL_Delay(1000);
		}
		//printf("serialControlThread Cnt = %d\n", ++cnt);
		//vTaskDelay(pdMS_TO_TICKS(1000));
		//HAL_Delay(1000);
	}
}

void SerialControl_start(uint8_t taskTag, QueueHandle_t queue){
	hMainQueue = queue;
	hTaskTag = taskTag;
  osThreadDef(SerialControlTask, serialControlThread, osPriorityNormal, 0, 256);
  serialControlTaskHandle = osThreadCreate(osThread(SerialControlTask), NULL);
}
