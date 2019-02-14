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
bool jsonPackReqHeartbeat(RpcPacket *rpcPacket);
bool jsonPackReqMeterValues(RpcPacket *rpcPacket, RequestMeterValues *req);
bool jsonPackReqStartTransaction(RpcPacket *rpcPacket, RequestStartTransaction *req);
bool jsonPackReqStatusNotification(RpcPacket *rpcPacket, RequestStatusNotification *req);
bool jsonPackReqStopTransaction(RpcPacket *rpcPacket, RequestStopTransaction *req);

bool jsonPackConfChangeConfiguration(RpcPacket *rpcPacket, ConfChangeConfiguration *conf);
bool jsonPackConfGetConfiguration(RpcPacket *rpcPacket, ConfGetConfiguration *conf);
bool jsonPackConfUnlockConnector(RpcPacket *rpcPacket, ConfUnlockConnector *conf);

bool jsonUnpackConfAuthorize(cJSON* json, ConfAuthorize *conf);
bool jsonUnpackConfBootNotification(cJSON* json, ConfBootNotifiaction *conf);
bool jsonUnpackConfHeartbeat(cJSON* json, ConfHeartbeat *conf);
bool jsonUnpackConfStartTransaction(cJSON* json, ConfStartTransaction *conf);

//Request from Server
bool jsonUnpackReqChangeConfiguration(cJSON* json, RequestChangeConfiguration *req);
bool jsonUnpackReqGetConfiguration(cJSON* json, RequestGetConfiguration *req);
bool jsonUnpackReqReset(cJSON* json, RequestReset *req);
bool jsonUnpackReqUnlockConnector(cJSON* json, RequestUnlockConnector *req);

