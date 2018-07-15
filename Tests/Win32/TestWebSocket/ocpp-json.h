#ifndef _WIN32
#include <stdbool.h>
#else
#include "defTypes.h"
#endif

#include "ocpp.h"
#include "rpc.h"
#include "cJSON.h"

bool jsonPackReqAuthorize(RpcPacket *rpcPacket, RequestAuthorize *req);
bool jsonPackReqBootNotification(RpcPacket *rpcPacket, RequestBootNotification *req);
bool jsonPackReqStartTransaction(RpcPacket *rpcPacket, RequestStartTransaction *req);
bool jsonPackReqStatusNotification(RpcPacket *rpcPacket, RequestStatusNotification *req);
bool jsonPackReqStopTransaction(RpcPacket *rpcPacket, RequestStopTransaction *req);

bool jsonPackConfUnlockConnector(RpcPacket *rpcPacket, ConfUnlockConnector *conf);

bool jsonUnpackConfBootNotification(cJSON* json, ConfBootNotifiaction *conf);
bool jsonUnpackConfBootAuthorize(cJSON* json, ConfAuthorize *conf);
bool jsonUnpackConfStartTransaction(cJSON* json, ConfStartTransaction *conf);

bool jsonUnpackReqUnlockConnector(cJSON* json, RequestUnlockConnector *req);
