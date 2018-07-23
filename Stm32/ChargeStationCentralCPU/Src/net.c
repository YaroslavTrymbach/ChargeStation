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
#include "ocpp-json.h"
#include "cJSON.h"
#include "settings.h"
#include "netConn.h"

#define SERVER_HOST_DEF "192.168.1.69"

#define WEBSERVER_THREAD_PRIO    ( tskIDLE_PRIORITY + 5 )
#define READ_THREAD_PRIO (tskIDLE_PRIORITY + 5)

static QueueHandle_t hQueue;
static uint8_t hTaskTag;

int sock;
struct netconn *conn;
  
int remote_port = 19201;
char remote_host[64];
ip_addr_t destIPaddr;

osThreadId hNetThread;
osThreadId hReadThread;

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


SemaphoreHandle_t hGetEvent;

extern struct netif gnetif;

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

	if(!res)
		printf("Not accepted\n");

	return res;
}

/*
bool makeWebsocketHandshake(){
	logStr("makeWebsocketHandshake\r\n");

	setServerPort(remote_port);
	setServerHost(remote_host);
	buf_cnt_out = fillHandshakeRequest(http_buf_out);

	clear_http_buf_in();
	setActiveProtocol(ACTIVE_PROTOCOL_HTTP);

	send(sock, http_buf_out, buf_cnt_out, 0);

	if(xSemaphoreTake(hGetEvent, pdMS_TO_TICKS(1000)) != pdPASS){
		logStr("Handshake answer is not got\n");
		return false;
	}

	if(httpBufOverflow){
		logStr("httpBufOverflow\r\n");
		return false;
	}

	logStr("Handshake answer is got\r\n");

	
	logStr(http_buf_in);

	return true;
}*/

bool connectToServer(void){
	return true;
}

void readThread(void const * argument){
	int res, status, size;
	char s[512];
	char buf[256];
	char *sp;
	char bufWS[512];
	char bufRPC[512];
	RpcPacket rpcPacket;
	cJSON* jsonRoot;
	cJSON* jsonElement;
	
	logStr("StartReadThread\r\n");
	
	rpcPacket.payload = (unsigned char*)bufRPC;
	rpcPacket.payloadSize = 512;
	
	while(true){
		res = recv(sock, buf, 64, 0);
		if(res > 0){
			buf[res] = '\0';
		}
		else{
			logStr("ConnectionIsClosed\r\n");
			return;
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
			if(WebSocket_ProcessInputData(buf, res, &status)){
				if(status == WS_PROCESS_STATUS_FINISHED){
					printf("WebSocket frame is got\n");
					WebSocket_GetInputPayloadData(bufWS, 512, &size);
					printf("Frame size is %d\n", size);
					
					if(parseRpcInputData(bufWS, size, &rpcPacket)){
						printf("RPC parsing good\n");
						printf("MesType %c Id %s, payloadLength %d\n", rpcPacket.messageType, rpcPacket.uniqueId, rpcPacket.payloadLen);
						printf("Payload: %s\n", rpcPacket.payload);
						
						jsonRoot = cJSON_Parse2((char*)rpcPacket.payload);

						printf("JSON is parsed\n");

						jsonElement = jsonRoot->child;

						while(jsonElement != NULL){
							if(jsonElement->type == cJSON_String){
								printf("Field: %s, Value: %s\n", jsonElement->string, jsonElement->valuestring);
							}
							else if(jsonElement->type == cJSON_Number){
								printf("Field: %s, Value: %d\n", jsonElement->string, jsonElement->valueint);
							}
							else{
								printf("Field: %s\n", jsonElement->string);
							}
							jsonElement = jsonElement->next;
						}

						cJSON_Delete2(jsonRoot);
					}
					else{
						printf("RPC parsing failed!\n");
					}
				}
			}
			else{
				printf("WebSocket_ProcessInputData. Failed\n");
			}
		}
		else{
		  sprintf(s, "Recv. res = %d. data = %s\n", res, buf);
		  logStr(s);
		}
	}
	
}

void runReadThread(){
	osThreadDef(readTask, readThread, osPriorityNormal, 0, 1024);
  hReadThread = osThreadCreate(osThread(readTask), NULL);
}

void sendWebSocketTest(){
/*	char inData[256];
	char outData[256];
	char rpcData[256];
	int outLen;
	struct RequestBootNotificatation request;
	logStr("sendWebSocketTest\r\n");

	strcpy(request.chargePointVendor, "Stm32JSONVendor");
	strcpy(request.chargePointModel, "Stm32JSONModel");

	jsonPackReqBootNotification(&request, inData, &outLen);
	
//	fillRpcCallData(ACTION_BOOT_NOTIFICATION, inData, outLen, rpcData, &outLen);
	
	if(fillWebSocketClientSendData(rpcData, outLen, outData, &outLen) != WS_OK){
		logStr("fillWebSocketClientSendData failed\r\n");
		return;
	}
	
	send(sock, outData, outLen, 0);	
	logStr("WebSocket data is send\r\n");*/
}

void netThread(void const * argument){
	int res;
	uint32_t tick;
	char s[256];
	char buf[256];
	int webSocketLastTryConnectionTick = 0;
  ChargePointSetting* st;	
	st = Settings_get();
	
	//osDelay(50);
	
	logStr("startNetTask\r\n");
	
	NET_CONN_init();
	NET_CONN_setRemoteHost(st->serverHost);
	NET_CONN_setRemotePort(st->serverPort);
	
	hGetEvent = xSemaphoreCreateBinary();
	
	//MX_LWIP_Init();
	//MX_LWIP_InitMod();
	/*
	
	logStr("Socket init\r\n");
	//Create socket
  sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock == -1){
		logStr("Could not create socket\r\n");
		return;
  }
  logStr("Socket created\r\n");
	
	strcpy(remote_host, SERVER_HOST_DEF); 
	
	server.sin_addr.s_addr = inet_addr(remote_host);
  server.sin_family = AF_INET;
  server.sin_port = htons( remote_port );
	
	sprintf(s, "Try to connect to %s:%d\r\n", remote_host, remote_port);
	logStr(s);
 
  //Connect to remote server
  if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0){
       logStr("connect failed\r\n");
       return;
  }
     
  logStr("Connected\r\n");
	
	strcpy(s, "12345");
	//send(sock, s, strlen(s), 0);
	
	runReadThread();
	
	if(makeWebsocketHandshake()){
		osDelay(1);
		setActiveProtocol(ACTIVE_PROTOCOL_WEBSOCKET);
		WebSocket_ClearInBuffer();
		sendWebSocketTest();
	}*/
	
	for(;;){
		if(!NET_CONN_isConnected()){
			if(NET_CONN_connect()){
				printf("Connected to server\n");
			}
			else{
				osDelay(100);
			}
		}
		
		if(NET_CONN_isConnected() && (!webSocketConnected)){
			tick = HAL_GetTick();
			if((tick - webSocketLastTryConnectionTick) >= 1000){
				webSocketLastTryConnectionTick = tick;
				webSocketConnected = makeWebsocketHandshake();
			}
		}
    osDelay(10);
		//send(sock, s, strlen(s), 0);
  }
	

}

static void init(void *arg){
	int res;
	char s[256];
	
	logStr("Init function\r\n");
	
	/*
	logStr("Socket init\r\n");
	//Create socket
  sock = socket(AF_INET , SOCK_STREAM , 0);
  if (sock == -1){
		logStr("Could not create socket\r\n");
		return false;
  }
  logStr("Socket created\r\n");*/
	
	// Create a new TCP connection handle 
  /*conn = netconn_new(NETCONN_TCP);
	if(conn == NULL){
		logStr("Failed create conn\r\n");
		return;
	}
	logStr("New connection is created\r\n");
	
	IP4_ADDR( &destIPaddr, 192, 168, 1, 69);
	
	res = netconn_connect(conn, &destIPaddr, remote_port);
	//res = 1;
	
	if(res == ERR_OK){
		logStr("Connected\r\n");
	}
	else{
		sprintf(s, "Connected failed. Res = %d\r\n", res);
		logStr(s);
	}*/
	
	//return true;
}

void NET_start(uint8_t taskTag, QueueHandle_t queue){
	hTaskTag = taskTag;
	hQueue = queue;	
	
	osThreadDef(netTask, netThread, osPriorityNormal, 0, 1024);
  hNetThread = osThreadCreate(osThread(netTask), NULL);
}

void NET_changeLocalIp(void){
	NET_CONN_change_local_ip();
}

void NET_test(void){
	uint32_t phyreg;
	printf("NET_test\n");
	HAL_ETH_ReadPHYRegister(&heth, PHY_BSR, &phyreg);
	if (phyreg & PHY_LINKED_STATUS){
		printf("Link is on\n");
	}
	else{
		printf("Link is off\n");
	}
}

void ethernetif_notify_conn_changed(struct netif *netif){
	printf("ethernetif_notify_conn_changed\n");
}