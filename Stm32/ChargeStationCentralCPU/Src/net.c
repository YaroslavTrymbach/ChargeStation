#include <stdbool.h>
#include "log.h"
#include "lwip/sockets.h"
#include "lwip/api.h"
#include "string.h"
#include <stdio.h>
#include "lwip.h"

#include "net.h"
#include "WebSocket.h"
#include "rpc.h"
#include "ocpp.h"
#include "ocppConfiguration.h"
#include "ocppConfigurationDef.h"
#include "ocpp-json.h"
#include "cJSON.h"
#include "settings.h"
#include "netConn.h"
#include "device.h"
#include "chargePointTime.h"
#include "tasks.h"
#include "chargePoint.h"
#include "connector.h"
#include "chargePointSettings.h"

#define WEBSERVER_THREAD_PRIO    ( tskIDLE_PRIORITY + 5 )
#define READ_THREAD_PRIO (tskIDLE_PRIORITY + 5)

static QueueHandle_t hMainQueue;
static uint8_t hTaskTag;
  
int remote_port = 19201;
char remote_host[64];

osThreadId hNetThread;
osThreadId hReadThread;

QueueHandle_t hNetInputQueue = NULL;

#define ACTIVE_PROTOCOL_NONE      0
#define ACTIVE_PROTOCOL_HTTP      1
#define ACTIVE_PROTOCOL_WEBSOCKET 2

int buf_cnt_in = 0;
int buf_cnt_out = 0;

#define HTTP_BUF_IN_SIZE 1024
#define HTTP_BUF_OUT_SIZE 1024

char http_buf_in[HTTP_BUF_IN_SIZE];
char http_buf_out[HTTP_BUF_OUT_SIZE];

char *buf_pos_in = http_buf_in;
char *buf_pos_out = http_buf_out;


//bool waitHttp = false;
int ActiveProtocol = ACTIVE_PROTOCOL_NONE;
bool httpBufOverflow = false;
bool webSocketConnected = false;
bool stationAccepted = false;

int bootNotificationInterval = 10000;
int heartbeatInterval = 5000;
int retryWebSocketHandshakeInterval = 0;

int SendCallAction = 0;
char* SendCallId[37];

SemaphoreHandle_t hGetEvent;
SemaphoreHandle_t hCallAnswerEvent;
SemaphoreHandle_t hReadThreadSuspendEvent;

bool isReadThreadNeedSuspend = false;

int authTagId;

extern struct netif gnetif;

static ChargePointConnector *activeConnector;

extern OcppConfigurationVaried ocppConfVaried;
extern OcppConfigurationFixed ocppConfFixed;
extern OcppConfigurationRestrict ocppConfRestrict;

void sendMessageToServer(RpcPacket* packet);
void sendMessageToMainDispatcher(GeneralMessage *message);

void clear_http_buf_in(){
	buf_pos_in = http_buf_in;
	buf_cnt_in = 0;
	httpBufOverflow = false;
}

void setActiveProtocol(int newProtocol){
	ActiveProtocol = newProtocol;
}

bool makeWebsocketHandshake(){
	bool res = false;
	WebSocketConnectionParams params;
	WebSocketHttpHeader answer;
	ChargePointSetting *st;
	
	retryWebSocketHandshakeInterval = 15000;
	
	printf("makeWebsocketHandshake\n");
	
	st = Settings_get();

	params.server_port = st->serverPort;
	strcpy(params.server_host, st->serverHost);
	sprintf(params.uri, "%s%s", st->serverUri, st->ChargePointId);
	buf_cnt_out = fillHandshakeRequest(&params, http_buf_out);

	clear_http_buf_in();
	setActiveProtocol(ACTIVE_PROTOCOL_HTTP);

	//send(sock, http_buf_out, buf_cnt_out, 0);
	NET_CONN_send(http_buf_out, buf_cnt_out);
	
	if(xSemaphoreTake(hGetEvent, pdMS_TO_TICKS(1000)) != pdPASS){
		printf("Handshake answer is not got\n");
		return false;
	}

	if(httpBufOverflow){
		printf("httpBufOverflow\n");
		return false;
	}

	printf("Handshake answer is got\n");

	if(WebSocket_parseHttpAnswerHeader(http_buf_in, &answer)){
		printf("StatusCode = %d\n", answer.StatusCode);
		if(answer.StatusCode == 101){
			res = true;
		}
	}

	//printf("%s", http_buf_in);
	if(res){
		setActiveProtocol(ACTIVE_PROTOCOL_WEBSOCKET);
		WebSocket_ClearInBuffer();
	}
	else
		printf("Not accepted\n");

	return res;
}


bool connectToServer(void){
	return true;
}

void sendPongFrame(char* appData, int appDataLen){
	char outData[512];
	int outLen;

	if(fillWebSocketPongData(appData, appDataLen, outData, &outLen) != WS_OK){
		printf("fillWebSocketPongData failed\n");
		return;
	}

	NET_CONN_send(outData, outLen);
}


void sendMessageToServer(RpcPacket* packet){
	char outData[512];
	char rpcData[512];
	int outLen;

	fillRpcCallData(packet, rpcData, &outLen);

	SendCallAction = packet->action;
	memcpy(SendCallId, packet->uniqueId, 37);
	
	if(fillWebSocketClientSendData(rpcData, outLen, outData, &outLen) != WS_OK){
		printf("fillWebSocketClientSendData failed\n");
		return;
	}
	
	NET_CONN_send(outData, outLen);
	//printf("WebSocket data is send\n");

	if(xSemaphoreTake(hCallAnswerEvent, pdMS_TO_TICKS(1000)) != pdPASS){
		printf("CallAnswer is not got\n");
		return;
	}
	
	//printf("Call answer is got\n");
}

void sendConfMessageToServer(RpcPacket* packet){
	char outData[512];
	char rpcData[512];
	int outLen;

	fillRpcCallResultData(packet, rpcData, &outLen);
	
	if(fillWebSocketClientSendData(rpcData, outLen, outData, &outLen) != WS_OK){
		printf("fillWebSocketClientSendData failed\n");
		return;
	}
	
	NET_CONN_send(outData, outLen);	
	printf("Conf message is send\n");
}

void sendMessageToMainDispatcher(GeneralMessage *message){
	message->sourceTag = hTaskTag;
	xQueueSend(hMainQueue, message, 10);
}

void processReqGetConfiguration(RpcPacket* packet, cJSON* json){
	int i;
	RequestGetConfiguration request;
	
	char jsonData[512];
	ConfGetConfiguration conf;
	RpcPacket rpcPacket;
	CiString50TypeListItem *lastUnKey;
	CiString50TypeListItem *unKey;
	KeyValueListItem *lastConfKey;
	KeyValueListItem *confKey;
	bool keyPassed;
	char* sAnswer;

	jsonUnpackReqGetConfiguration(json, &request);

	printf("GetConfiguration. Cnt = %d\n", request.keySize);
	

	rpcPacket.payload = (unsigned char*)jsonData;
	rpcPacket.payloadSize = 512;
	strcpy(rpcPacket.uniqueId, packet->uniqueId);

	conf.unknownKey = NULL;
	conf.configurationKey = NULL;
	
	
	
	for(i = 0; i < request.keySize; i++){
		printf("Key %d: %s\n", i + 1, request.key[i]);

		keyPassed = true;
		switch(occpGetConfigKeyFromString(request.key[i])){
			case CONFIG_KEY_AUTHORIZE_REMOTE_TX_REQUESTS:
				confKey = ocppCreateKeyValueBool(CONFIG_KEY_AUTHORIZE_REMOTE_TX_REQUESTS, ocppConfRestrict.authorizeRemoteTxRequestsReadOnly, 
					ocppConfRestrict.authorizeRemoteTxRequestsReadOnly ? ocppConfFixed.authorizeRemoteTxRequests:  ocppConfVaried.authorizeRemoteTxRequests); 
				break;
			case CONFIG_KEY_GET_CONFIGURATION_MAX_KEYS:
				confKey = ocppCreateKeyValueInt(CONFIG_KEY_GET_CONFIGURATION_MAX_KEYS, true, CONFIGURATION_GET_MAX_KEYS);
				break;
			case CONFIG_KEY_CONNECTION_TIMEOUT:
				confKey = ocppCreateKeyValueInt(CONFIG_KEY_CONNECTION_TIMEOUT, false, ocppConfVaried.connectionTimeOut);
				break;
			case CONFIG_KEY_LOCAL_AUTHORIZE_OFFLINE:
				confKey = ocppCreateKeyValueBool(CONFIG_KEY_LOCAL_AUTHORIZE_OFFLINE, false, ocppConfVaried.localAuthorizeOffline);
				break;
			case CONFIG_KEY_LOCAL_PRE_AUTHORIZE:
				confKey = ocppCreateKeyValueBool(CONFIG_KEY_LOCAL_PRE_AUTHORIZE, false, ocppConfVaried.localPreAuthorize);
				break;
			case CONFIG_KEY_NUMBER_OF_CONNECTORS:
				confKey = ocppCreateKeyValueInt(CONFIG_KEY_NUMBER_OF_CONNECTORS, true, CONFIGURATION_NUMBER_OF_CONNECTORS);
				break;
			case CONFIG_KEY_SUPPORTED_FEATURE_PROFILES:
				sAnswer = ocppCreateProfileCSL(OCPP_PROFILE_MASK);
				confKey = ocppCreateKeyValueString(CONFIG_KEY_SUPPORTED_FEATURE_PROFILES, true, sAnswer);
				free(sAnswer);
				break;

			case CONFIG_KEY_LOCAL_AUTH_LIST_ENABLED:
				confKey = ocppCreateKeyValueBool(CONFIG_KEY_LOCAL_AUTH_LIST_ENABLED, true, LOCAL_AUTH_LIST_ENABLED);
				break;
			case CONFIG_KEY_LOCAL_AUTH_LIST_MAX_LENGTH:
				confKey = ocppCreateKeyValueInt(CONFIG_KEY_LOCAL_AUTH_LIST_MAX_LENGTH, true, LOCAL_AUTH_LIST_MAX_LENGTH);
				break;
			case CONFIG_KEY_SEND_LOCAL_LIST_MAX_LENGTH:
				confKey = ocppCreateKeyValueInt(CONFIG_KEY_SEND_LOCAL_LIST_MAX_LENGTH, true, LOCAL_AUTH_LIST_MAX_LENGTH);
				break;
			default:
				keyPassed = false;
		}

		if(keyPassed){
			//Add to configuration key
			if(conf.configurationKey == NULL){
				conf.configurationKey = confKey;
			}
			else{
				lastConfKey->next = confKey;
			}
			lastConfKey = confKey;			
		}
		else{
			//Add to unknowKey
			unKey = malloc(sizeof(CiString50TypeListItem));
			unKey->next = NULL;
			strcpy(unKey->data, request.key[i]);
			if(conf.unknownKey == NULL){
				conf.unknownKey = unKey;
			}
			else{
				lastUnKey->next = unKey;
			}
			lastUnKey = unKey;
		}
		
	}

	printf("send GetConfiguration confirmation\n");
	
	jsonPackConfGetConfiguration(&rpcPacket, &conf);

	//Free memory in conf
	ocppFreeKeyValueList(conf.configurationKey);
	ocppFreeCiString50TypeList(conf.unknownKey);

	sendConfMessageToServer(&rpcPacket);	
}

void processConfBootNotification(cJSON* json){
	ConfBootNotifiaction conf;
	GeneralMessage message;
	jsonUnpackConfBootNotification(json, &conf);

	printf("Server datetime: %.2d.%.2d.%.4d %.2d:%.2d:%.2d\n", 
		conf.currentTime.tm_mday, conf.currentTime.tm_mon, conf.currentTime.tm_year,
		conf.currentTime.tm_hour, conf.currentTime.tm_min, conf.currentTime.tm_sec);

	if(conf.status == REGISTRATION_STATUS_ACCEPTED){
		printf("Station is accepted\n");
		//Synchronize time
		setCurrentTime(&conf.currentTime);
		
		stationAccepted = true;
		heartbeatInterval = conf.interval*1000;
	}
	else{
		printf("Station is rejected\n");
		bootNotificationInterval = conf.interval*1000;
	}
	
	//Send message to Main dispatcher
	message.messageId = MESSAGE_NET_SERVER_ACCEPT;
	message.param1 = (stationAccepted) ? 1 : 0;
	sendMessageToMainDispatcher(&message);
}

void processConfHeartbeat(cJSON* json){
	ConfHeartbeat conf;
	jsonUnpackConfHeartbeat(json, &conf);

	printf("Server datetime: %.2d.%.2d.%.4d %.2d:%.2d:%.2d\n", 
		conf.currentTime.tm_mday, conf.currentTime.tm_mon, conf.currentTime.tm_year,
		conf.currentTime.tm_hour, conf.currentTime.tm_min, conf.currentTime.tm_sec);
	
	//Synchronize time
	setCurrentTime(&conf.currentTime);
}

void processConfAuthorize(cJSON* json){
	ConfAuthorize conf;
	GeneralMessage message;
	jsonUnpackConfAuthorize(json, &conf);
	
	message.messageId = MESSAGE_NET_AUTHORIZE;
	message.param1 = (conf.idTagInfo.status == AUTHORIZATION_STATUS_ACCEPTED) ? 1 : 0;
	message.param2 = authTagId;
	sendMessageToMainDispatcher(&message);
}

void processConfStartTransaction(cJSON* json){
	ConfStartTransaction conf;
	jsonUnpackConfStartTransaction(json, &conf);

	if(conf.idTagInfo.status == AUTHORIZATION_STATUS_ACCEPTED){
		chargeTransaction_accept(&activeConnector->chargeTransaction, conf.transactionId);
	}
	else{
		//StartTransaction is rejected
	}
}

void processRPCPacket(RpcPacket* packet){
	cJSON* jsonRoot;

	jsonRoot = cJSON_Parse2((char*)packet->payload);

	if(jsonRoot == NULL){
		printf("JSON parisng failed");
		return;
	}

	if(packet->messageType == MES_TYPE_CALLRESULT){
		if(memcmp(packet->uniqueId, SendCallId, 36) == 0){
			//printf("Get call result\n");
		
			switch(SendCallAction){
				case ACTION_AUTHORIZE:
					processConfAuthorize(jsonRoot);
					break;
				case ACTION_BOOT_NOTIFICATION:
					processConfBootNotification(jsonRoot);
					break;
				case ACTION_HEARTBEAT:
					processConfHeartbeat(jsonRoot);
					break;
				case ACTION_START_TRANSACTION:
					processConfStartTransaction(jsonRoot);
					break;
			}

			xSemaphoreGive(hCallAnswerEvent);
		}
	}
	else if (packet->messageType == MES_TYPE_CALL){
		//Запрос от сервера
		switch(packet->action){
			case ACTION_GET_CONFIGURATION:
				processReqGetConfiguration(packet, jsonRoot);	
				break;
			case ACTION_UNLOCK_CONNECTOR:
				//processReqUnlockConnector(packet, jsonRoot);
				break;
		}

	}

	cJSON_Delete2(jsonRoot);

}

static void readThread(void const * argument){
	int res, status, size;
	char s[128];
	char buf[256];
	char *sp;
	char bufWS[512];
	char bufRPC[512];
	RpcPacket rpcPacket;
	WebSocketInputDataState webSocketState;

	
	printf("startNetReadThread\n");
	
	rpcPacket.payload = (unsigned char*)bufRPC;
	rpcPacket.payloadSize = 512;
	
	while(true){
		if(isReadThreadNeedSuspend){
			printf("ReadThread suspend\n");
			xSemaphoreGive(hReadThreadSuspendEvent);
			osThreadSuspend(NULL);
		}
		res = NET_CONN_recv(buf, 500);

		if(res > 0){
			buf[res] = '\0';
		}
		else{
			if(res == 0){
				continue;
			}
			else{
				printf("ConnectionIsClosed\n");
				osThreadSuspend(NULL);
				continue;
				//return;
			}
		}
		//Обработка полученного результата
		if((ActiveProtocol == ACTIVE_PROTOCOL_HTTP) && (!httpBufOverflow)){
			if((buf_cnt_in + res) > HTTP_BUF_IN_SIZE){
				httpBufOverflow = true;
				xSemaphoreGive(hGetEvent);
			}
			else{
				memcpy(buf_pos_in, buf, res);
				buf_cnt_in += res;
				buf_pos_in += res;

				//Ищем признак окончания блока хедеров
				buf_pos_in[0] = '\0';
				sp = strstr(http_buf_in, "\r\n\r\n");
				if(sp != NULL){
					//Нашли!!!
					sp[4] = '\0';
					xSemaphoreGive(hGetEvent);
				}
			}
		}
		else if(ActiveProtocol == ACTIVE_PROTOCOL_WEBSOCKET){
			if(WebSocket_ProcessInputData(buf, res, &webSocketState)){
				if(webSocketState.status == WS_PROCESS_STATUS_FINISHED){
					if(webSocketState.opCode == WEB_SOCKET_OPCODE_PING){
						printf("WebSocket get ping\n");
						WebSocket_GetInputPayloadData(bufWS, 512, &size);
						sendPongFrame(bufWS, size);
					}
					else{
						//printf("WebSocket frame is got\n");
						WebSocket_GetInputPayloadData(bufWS, 512, &size);
						//printf("Frame size is %d\n", size);
						
						if(parseRpcInputData(bufWS, size, &rpcPacket)){
							/*printf("RPC parsing good\n");
							printf("MesType %c Id %s, payloadLength %d\n", rpcPacket.messageType, rpcPacket.uniqueId, rpcPacket.payloadLen);
							printf("Payload: %s\n", rpcPacket.payload);*/

							processRPCPacket(&rpcPacket);
						}
						else{
							printf("RPC parsing failed!\n");
						}
					}
					WebSocket_ClearInBuffer();
				}
			}
			else{
				printf("WebSocket_ProcessInputData. Failed\n");
			}
		}
		else{
		  sprintf(s, "Recv. res = %d. data = %s\n", res, buf);
		  printf(s);
		}
	}
	
}

bool createReadThread(){
	osThreadDef(readTask, readThread, osPriorityNormal, 0, 2048);
  hReadThread = osThreadCreate(osThread(readTask), NULL);
	if(hReadThread != NULL)
		osThreadSuspend(hReadThread);
	return (hReadThread != NULL);
}

void runReadThread(void){
	isReadThreadNeedSuspend = false;
	osThreadResume(hReadThread);
}

void suspendReadThread(void){
	isReadThreadNeedSuspend = true;
	xSemaphoreTake(hReadThreadSuspendEvent, pdMS_TO_TICKS(1000));
}

void sendAuthorizationRequest(uint32_t tagId){
	char jsonData[512];
	RequestAuthorize request;
	RpcPacket rpcPacket;

	rpcPacket.payload = (unsigned char*)jsonData;
	rpcPacket.payloadSize = 512;

	printf("send autorize request\n");
	
	sprintf(request.idTag, "%.8X", tagId);

	jsonPackReqAuthorize(&rpcPacket, &request);

	sendMessageToServer(&rpcPacket);
}

void sendBootNotification(void){
	char jsonData[512];
	
	RequestBootNotification request;
	RpcPacket rpcPacket;

	rpcPacket.payload = (unsigned char*)jsonData;
	rpcPacket.payloadSize = 512;

	printf("sendBootNotification\n");

	strcpy(request.chargePointVendor, CHARGE_POINT_VENDOR);
	strcpy(request.chargePointModel, CHARGE_POINT_MODEL);

	jsonPackReqBootNotification(&rpcPacket, &request);

	sendMessageToServer(&rpcPacket);
}

void sendHeartbeatRequest(){
	char jsonData[512];
	
	RpcPacket rpcPacket;

	rpcPacket.payload = (unsigned char*)jsonData;
	rpcPacket.payloadSize = 512;

	printf("sendBootNotification\n");

	jsonPackReqHeartbeat(&rpcPacket);

	sendMessageToServer(&rpcPacket);
}

void sendStatusNotification(int connectorId, int status, int errorCode){
	char jsonData[512];
	
	RequestStatusNotification request;
	RpcPacket rpcPacket;

	rpcPacket.payload = (unsigned char*)jsonData;
	rpcPacket.payloadSize = 512;

	printf("send status notification\n");
	
	request.connectorId = connectorId;
	request.status = status;
	request.errorCode = errorCode;		

	jsonPackReqStatusNotification(&rpcPacket, &request);

	sendMessageToServer(&rpcPacket);
}

void sendStartTransaction(ChargePointConnector *conn){
	char jsonData[512];
	RequestStartTransaction request;
	RpcPacket rpcPacket;

	rpcPacket.payload = (unsigned char*)jsonData;
	rpcPacket.payloadSize = 512;

	printf("send startTransaction request. connId = %d\n", conn->id);
	
	request.connectorId = conn->id;
	sprintf(request.idTag, "%.8X", conn->userTagId);
	request.meterStart = conn->meterValue;
	request.useReservationId = false;
	getCurrentTime(&request.timestamp);
	
	activeConnector = conn;

	jsonPackReqStartTransaction(&rpcPacket, &request);

	sendMessageToServer(&rpcPacket);
}

void sendStopTransaction(ChargePointConnector *conn){
	char jsonData[512];
	RequestStopTransaction request;
	RpcPacket rpcPacket;

	rpcPacket.payload = (unsigned char*)jsonData;
	rpcPacket.payloadSize = 512;

	printf("send stopTransaction request\n");

	request.transactionId = chargeTransaction_getId(&(conn->chargeTransaction));
	request.meterStop = conn->chargeTransaction.stopMeterValue;
	getCurrentTime(&request.timestamp);
	request.useIdTag = false;
	request.useReason = false;

	jsonPackReqStopTransaction(&rpcPacket, &request);

	sendMessageToServer(&rpcPacket);
}

void reconnect(){
	printf("NET: Reconnect\n");
	suspendReadThread();
	NET_CONN_disconnect();
	webSocketConnected = false;
	stationAccepted = false;
	bootNotificationInterval = 0;
}

#define GET_PARAM_BYTE0(value) value & 0xFF
#define GET_PARAM_BYTE1(value) (value >> 8)  & 0xFF
#define GET_PARAM_BYTE2(value) (value >> 16) & 0xFF
#define GET_PARAM_BYTE3(value) (value >> 24) & 0xFF

void netThread(void const * argument){
	uint32_t tick;
	int webSocketLastTryConnectionTick = 0;
	int bootNotificationTick = 0;
	int heartbeatTick = 0;
  ChargePointSetting* st;	
	NetInputMessage message;
	int status, connId, errorCode;
	
	st = Settings_get();
	
	//osDelay(50);
		
	printf("startNetTask\n");
	
	NET_CONN_init();
	NET_CONN_setRemoteHost(st->serverHost);
	NET_CONN_setRemotePort(st->serverPort);
	
	hGetEvent = xSemaphoreCreateBinary();
	hCallAnswerEvent = xSemaphoreCreateBinary(); 
	hNetInputQueue = xQueueCreate(16, sizeof(NetInputMessage));
	hReadThreadSuspendEvent = xSemaphoreCreateBinary();  
	if(!createReadThread()){
		printf("NET createReadThread failed. FreeHeap = %d\n", xPortGetFreeHeapSize());
	}
	
	for(;;){
		if(!NET_CONN_isConnected()){
			webSocketConnected = false;
			stationAccepted = false;
			if(NET_CONN_connect()){
				printf("Connected to server\n");
				runReadThread();
			}
			else{
				osDelay(100);
				continue;
			}
		}
		
		if(NET_CONN_isConnected() && (!webSocketConnected)){
			tick = HAL_GetTick();
			if((tick - webSocketLastTryConnectionTick) >= retryWebSocketHandshakeInterval){
				webSocketLastTryConnectionTick = tick;
				webSocketConnected = makeWebsocketHandshake();
			}
		}
		
		if(webSocketConnected){
			tick = HAL_GetTick();
			if(stationAccepted){
				if((tick - heartbeatTick) >= heartbeatInterval){
					heartbeatTick = tick;
					sendHeartbeatRequest();
				}
			}
			else{
				if((tick - bootNotificationTick) >= bootNotificationInterval){
					bootNotificationTick = tick;
					heartbeatTick = tick;
					sendBootNotification();
				}		
			}
			//stationAccepted = true; //!Debug
		}
		
		while(xQueueReceive(hNetInputQueue, &message, 0) == pdPASS){
			switch(message.messageId){
				case NET_INPUT_MESSAGE_RECONNECT:
					reconnect();
				  //return;
					break;
				case NET_INPUT_MESSAGE_AUTHORIZE:
					authTagId = message.param1;
					sendAuthorizationRequest(authTagId);
					break;
				case NET_INPUT_MESSAGE_SEND_CONNECTOR_STATUS:
					connId = GET_PARAM_BYTE0(message.param1);
				  status = GET_PARAM_BYTE1(message.param1);
				  errorCode = GET_PARAM_BYTE2(message.param1);
					sendStatusNotification(connId, status, errorCode);
					break;
				case NET_INPUT_MESSAGE_SEND_CHARGE_POINT_STATUS:
					connId = 0;
				  status = ChargePoint_getStatusState();
				  if(status == CHARGE_POINT_STATUS_FAULTED)
						errorCode = ChargePoint_getStatusLastErrorCode();
					else
						errorCode = CHARGE_POINT_ERROR_CODE_NO_ERROR;
				  sendStatusNotification(connId, status, errorCode);
					break;
				case NET_INPUT_MESSAGE_START_TRANSACTION:
					sendStartTransaction((ChargePointConnector*)message.param1);
					break;
				case NET_INPUT_MESSAGE_STOP_TRANSACTION:
					sendStopTransaction((ChargePointConnector*)message.param1);
					break;
			}
			
			//stackLeft = uxTaskGetStackHighWaterMark(NULL);		
			//printf("NetTask event:%d stackLeft:%d\n", message.messageId, stackLeft);
		}
    osDelay(10);
		//send(sock, s, strlen(s), 0);
  }
}


void NET_start(uint8_t taskTag, QueueHandle_t queue){
	hTaskTag = taskTag;
	hMainQueue = queue;
	
	osThreadDef(netTask, netThread, osPriorityNormal, 0, 1024);
  hNetThread = osThreadCreate(osThread(netTask), NULL);
}

void NET_changeLocalIp(void){
	NET_CONN_change_local_ip();
}

void NET_sendInputMessage(NetInputMessage *message){
	if(hNetInputQueue != NULL){
		xQueueSend(hNetInputQueue, message, 10);
	}
}

void NET_test(void){
	uint32_t phyreg;
	uint32_t phyreg2;
	
	printf("NET_test\n");
	HAL_ETH_ReadPHYRegister(&heth, PHY_BSR, &phyreg);
	if (phyreg & PHY_LINKED_STATUS){
		printf("Link is on\n");
	}
	else{
		printf("Link is off\n");
	}
	
	HAL_ETH_ReadPHYRegister(&heth, 29, &phyreg);
	HAL_ETH_ReadPHYRegister(&heth, 30, &phyreg2);
	printf("1 ISFR = 0x%.2X, IMR = 0x%.2X\n", phyreg, phyreg2);
	
	HAL_ETH_ReadPHYRegister(&heth, 29, &phyreg);
	HAL_ETH_ReadPHYRegister(&heth, 30, &phyreg2);
	printf("2 ISFR = 0x%.2X, IMR = 0x%.2X\n", phyreg, phyreg2);
}

bool NET_is_station_accepted(void){
	return stationAccepted;
}

void ethernetif_notify_conn_changed(struct netif *netif){
	printf("ethernetif_notify_conn_changed\n");
}
