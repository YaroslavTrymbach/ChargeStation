#ifndef _WIN32
#include <stdbool.h>
#include <stdint.h>
#else
#include "defTypes.h"
#endif

#define WEB_SOCKET_OPCODE_TEXT 1
#define WEB_SOCKET_OPCODE_PING 9
#define WEB_SOCKET_OPCODE_PONG 10

#define WebSocketStatus int

#define	WS_OK    0
#define	WS_ERROR 1

#define WS_PROCESS_STATUS_WAITING  0
#define WS_PROCESS_STATUS_FINISHED 1

typedef struct _WebSocketInputDataState{
	int status;
	int opCode;
}WebSocketInputDataState;

typedef struct _WebSocketHttpHeader{
	int StatusCode;
}WebSocketHttpHeader;

typedef struct _WebSocketConnectionParams{
	int server_port;
	char server_host[64];
	char uri[256];
}WebSocketConnectionParams;

void foo(void);

int fillHandshakeRequest(WebSocketConnectionParams *params, char *buf);
WebSocketStatus fillWebSocketClientSendData(char* inData, int inlen, char* outData, int* outLen);
WebSocketStatus fillWebSocketPongData(char* inData, int inLen, char* outData, int *outLen);
void WebSocket_ClearInBuffer();
bool WebSocket_ProcessInputData(uint8_t *data, int dataLen, WebSocketInputDataState *state);
bool WebSocket_GetInputPayloadData(uint8_t *buf, int bufSize, int *outSize);

bool WebSocket_parseHttpAnswerHeader(char* data, WebSocketHttpHeader* outHeader);

