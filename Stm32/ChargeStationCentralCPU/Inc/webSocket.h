#ifndef _WIN32
#include <stdbool.h>
#include <stdint.h>
#else
#include "defTypes.h"
#endif

#define WebSocketStatus int

#define	WS_OK    0
#define	WS_ERROR 1

#define WS_PROCESS_STATUS_WAITING  0
#define WS_PROCESS_STATUS_FINISHED 1

void foo(void);
void setServerPort(int port);
void setServerHost(char *host);
int fillHandshakeRequest(char *buf);
WebSocketStatus fillWebSocketClientSendData(char* inData, int inlen, char* outData, int* outLen);
void WebSocket_ClearInBuffer();
bool WebSocket_ProcessInputData(uint8_t *data, int dataLen, int* status);
bool WebSocket_GetInputPayloadData(uint8_t *buf, int bufSize, int *outSize);

