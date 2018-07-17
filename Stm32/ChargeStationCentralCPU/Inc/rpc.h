#ifndef _WIN32
#include <stdbool.h>
#include <stdint.h>
#else
#include "defTypes.h"
#endif

#define RPC_GUID_MAX_LENGTH 36

typedef struct _RpcPacket{
	uint8_t messageType;
	uint8_t action;
	uint8_t uniqueId[RPC_GUID_MAX_LENGTH + 1];
	uint8_t* payload;
	uint32_t payloadLen;  //Actual data length in payload
	uint32_t payloadSize; //Size of buffer;
}RpcPacket;

bool fillRpcCallData(int action, char* payload, int payloadLen, char* outData, int* outLen);

bool parseRpcInputData(char* data, int dataLen, RpcPacket *outPacket);

//bool parseRpc
