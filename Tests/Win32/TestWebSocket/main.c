#include <stdio.h>
//#include "webSocket.h"
#include <winsock2.h>
#include <windows.h>
#include <string.h>
#include "deftypes.h"
#include "ocpp.h"
#include "rpc.h"
#include "cJson.h"
#include "tasks.h"
#include "ocpp-json.h"


//#define SERVER_PORT_NO 19200
//#define SERVER_HOST    "192.168.1.69"
//#define SERVER_PORT_NO 19201
//#define SERVER_HOST "127.0.0.1"
#define SERVER_HOST    "192.168.1.63"
//#define SERVER_HOST    "192.168.1.100"
//#define SERVER_HOST    "192.168.1.101"
#define SERVER_PORT_NO 8080

#define STATION_MODEL "YarModel777" //Must be accepted
//#define STATION_MODEL "YarModelBad"

#define STATION_TAG "1234"
//#define STATION_TAG "5678"

#define	WS_OK    0
#define	WS_ERROR 1

#define WS_PROCESS_STATUS_WAITING  0
#define WS_PROCESS_STATUS_FINISHED 1

SOCKET sock;

HANDLE hMainThread;
HANDLE hReadThread;

HANDLE hGetEvent;
HANDLE hCallAnswerEvent;

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

int ActiveProtocol = ACTIVE_PROTOCOL_NONE;
bool httpBufOverflow = false;

int SendCallAction = 0;
char* SendCallId[37];

bool isAcceptedByServer = false;
bool isAuthorized = false;

GeneralMessage message;
bool isMessageActive = false;

void finish(){
	TerminateThread(hReadThread, 0);
}

void clear_http_buf_in(){
	buf_pos_in = http_buf_in;
	buf_cnt_in = 0;
	httpBufOverflow = false;
}

void setActiveProtocol(int newProtocol){
	ActiveProtocol = newProtocol;
}


bool makeWebsocketHandshake(){
	printf("makeWebsocketHandshake\n");

	setServerPort(SERVER_PORT_NO);
	setServerHost(SERVER_HOST);
	buf_cnt_out = fillHandshakeRequest(http_buf_out);

	clear_http_buf_in();
	setActiveProtocol(ACTIVE_PROTOCOL_HTTP);

	send(sock, http_buf_out, buf_cnt_out, 0);

	if(WaitForSingleObject(hGetEvent, 1000) != WAIT_OBJECT_0){
		printf("Handshake answer is not got\n");
		return false;
	}

	if(httpBufOverflow){
		printf("httpBufOverflow\n");
		return false;
	}

	printf("Handshake answer is got\n");

	printf("%s", http_buf_in);

	return true;
}

void sendMessage(RpcPacket* packet){
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
	
	send(sock, outData, outLen, 0);	
	printf("WebSocket data is send\n");

	if(WaitForSingleObject(hCallAnswerEvent, 1000) != WAIT_OBJECT_0){
		printf("CallAnswer is not got\n");
		return;
	}
	
	printf("Call answer is got\n");
}

void sendConfMessage(RpcPacket* packet){
	char outData[512];
	char rpcData[512];
	int outLen;

	fillRpcCallResultData(packet, rpcData, &outLen);
	
	if(fillWebSocketClientSendData(rpcData, outLen, outData, &outLen) != WS_OK){
		printf("fillWebSocketClientSendData failed\n");
		return;
	}
	
	send(sock, outData, outLen, 0);	
	printf("Conf message is send\n");
}


void sendWebSocketTest(){
	char jsonData[512];
	
	int outLen;
	RequestBootNotification request;
	RpcPacket rpcPacket;

	rpcPacket.payload = (unsigned char*)jsonData;
	rpcPacket.payloadSize = 512;

	printf("sendWebSocketTest\n");

	strcpy(request.chargePointVendor, "YarJSONVendor");
	strcpy(request.chargePointModel, STATION_MODEL);

	jsonPackReqBootNotification(&rpcPacket, &request);

	//rpcPacket.action = ACTION_BOOT_NOTIFICATION;
	//rpcPacket.payloadLen = outLen;

	sendMessage(&rpcPacket);
}

void sendStatusNotification(int connectorId){
	char jsonData[512];
	
	int outLen;
	RequestStatusNotification request;
	RpcPacket rpcPacket;

	rpcPacket.payload = (unsigned char*)jsonData;
	rpcPacket.payloadSize = 512;

	printf("send status notification\n");
	
	request.connectorId = connectorId;
	if(connectorId != 3){
		request.errorCode = CHARGE_POINT_ERROR_CODE_NO_ERROR;
		request.status = CHARGE_POINT_STATUS_AVAILABLE;
	}
	else{
		request.errorCode = CHARGE_POINT_ERROR_CODE_CONNECTOR_LOCK_FAILURE;
		request.status = CHARGE_POINT_STATUS_FAULTED;
	}

	jsonPackReqStatusNotification(&rpcPacket, &request);

	sendMessage(&rpcPacket);
}
	
void sendAuthorizationRequest(){
	char jsonData[512];
	RequestAuthorize request;
	RpcPacket rpcPacket;

	rpcPacket.payload = (unsigned char*)jsonData;
	rpcPacket.payloadSize = 512;

	printf("send autorize request\n");
	
	strcpy(request.idTag, STATION_TAG);

	jsonPackReqAuthorize(&rpcPacket, &request);

	sendMessage(&rpcPacket);
}

void sendConfUnlockConnector(const char* uniqueId){
	char jsonData[512];
	ConfUnlockConnector conf;
	RpcPacket rpcPacket;

	rpcPacket.payload = (unsigned char*)jsonData;
	rpcPacket.payloadSize = 512;
	strcpy(rpcPacket.uniqueId, uniqueId);

	printf("send unlock connector confirmation\n");

	conf.status = UNLOCK_STATUS_UNLOCKED;
	jsonPackConfUnlockConnector(&rpcPacket, &conf);

	sendConfMessage(&rpcPacket);
}

void mainThread(){
	int i;
	if(!makeWebsocketHandshake()){
		finish();
		return;
	}

	setActiveProtocol(ACTIVE_PROTOCOL_WEBSOCKET);
	WebSocket_ClearInBuffer();
	sendWebSocketTest();

	if(!isAcceptedByServer){
		closesocket(sock);
		return;
	}

	//sendAuthorizationRequest();
	for(i = 1; i <= 4; i++){
		sendStatusNotification(i);
	}

	while(true){
		if(isMessageActive){
			if(message.sourceTag == TASK_TAG_SERVER){
				sendConfUnlockConnector(message.uniqueId);
			}
			isMessageActive = false;
		}
		Sleep(5);
	}

	Sleep(3000);
	closesocket(sock);
	Sleep(500);
}

void processConfAuthorize(cJSON* json){
	ConfAuthorize conf;
	jsonUnpackConfBootAuthorize(json, &conf);

	if(conf.idTagInfo.status == AUTHORIZATION_STATUS_ACCEPTED){
		printf("Authorization is accepted\n");
	}
	else{
		printf("Authorization is rejected\n");
	}
}

void processConfBootNotification(cJSON* json){
	ConfBootNotifiaction conf;
	jsonUnpackConfBootNotification(json, &conf);

	if(conf.status == REGISTRATION_STATUS_ACCEPTED){
		printf("Station is accepted\n");
		isAcceptedByServer = true;
	}
	else{
		printf("Station is rejected\n");
	}
}

void processReqUnlockConnector(RpcPacket* packet, cJSON* json){
	RequestUnlockConnector request;
	jsonUnpackReqUnlockConnector(json, &request);

	message.sourceTag = TASK_TAG_SERVER;
	message.messageId = ACTION_UNLOCK_CONNECTOR;
	message.param1 = request.connectorId;
	strcpy(message.uniqueId, packet->uniqueId);
	isMessageActive = true;
}

void processRPCPacket(RpcPacket* packet){
	cJSON* jsonRoot;
	cJSON* jsonElement;
	//if(rpcPacket.messageType == 

	jsonRoot = cJSON_Parse2((char*)packet->payload);

	if(jsonRoot == NULL){
		printf("JSON parisng failed");
		return;
	}


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

	if(packet->messageType == MES_TYPE_CALLRESULT){
		if(memcmp(packet->uniqueId, SendCallId, 36) == 0){
			printf("Get call result\n");
		
			switch(SendCallAction){
				case ACTION_AUTHORIZE:
					processConfAuthorize(jsonRoot);
					break;
				case ACTION_BOOT_NOTIFICATION:
					processConfBootNotification(jsonRoot);
					break;
			}

			SetEvent(hCallAnswerEvent);
		}
	}
	else if (packet->messageType == MES_TYPE_CALL){
		//Запрос от сервера
		switch(packet->action){
			case ACTION_UNLOCK_CONNECTOR:
				processReqUnlockConnector(packet, jsonRoot);
				break;
		}

	}

	cJSON_Delete2(jsonRoot);

}

void readThread(){
	//Sleep(100);
	//printf("readThread\n");
	int res, status, size;
	char s[256];
	char buf[512];
	char bufWS[512];
	char bufRPC[512];
	char *sp;
	RpcPacket rpcPacket;

	rpcPacket.payload = (unsigned char*)bufRPC;
	rpcPacket.payloadSize = 512;

	while(true){
		res = recv(sock, buf, 64, 0);
		if(res > 0){
			buf[res] = '\0';
		}
		else{
			printf("ConnectionIsClosed\n");
			return;
		}
		//Обработка полученного результата
		if((ActiveProtocol == ACTIVE_PROTOCOL_HTTP) && (!httpBufOverflow)){
			if((buf_cnt_in + res) > HTTP_BUF_IN_SIZE){
				httpBufOverflow = true;
				SetEvent(hGetEvent);
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
					SetEvent(hGetEvent);
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

						processRPCPacket(&rpcPacket);
					}
					else{
						printf("RPC parsing failed!\n");
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

bool initSocket(){
	WSADATA wsaData;
	WORD wVersionRequested;
	struct sockaddr_in server;
	wVersionRequested = MAKEWORD( 2, 0 );
	WSAStartup(wVersionRequested,&wsaData);

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1){
		printf("Could not create socket\n");
		return false;
	}
	printf("Socket created\n");
	
	server.sin_addr.s_addr = inet_addr(SERVER_HOST);
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT_NO);

	printf("Try to connect to server %s : %d\n", SERVER_HOST, SERVER_PORT_NO);

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0){
       printf("connect failed\r\n");
       return false;
	 }
     
	printf("Connected\r\n");
	return true;
}

int main(){
	printf("TestWebSocket\n");
	if(!initSocket()){
		return 1;
	}

	hGetEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	hCallAnswerEvent = CreateEvent(NULL, FALSE, FALSE, NULL); 

	hReadThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)readThread, NULL, 0, NULL);
	hMainThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)mainThread, NULL, 0, NULL);

	//foo();
	WaitForSingleObject(hMainThread, INFINITE);
	printf("Exit\n");
	return 0;
}