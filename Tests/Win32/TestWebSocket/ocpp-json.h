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

bool jsonPackConfChangeAvailability(RpcPacket *rpcPacket, ConfChangeAvailability *conf);
bool jsonPackConfChangeConfiguration(RpcPacket *rpcPacket, ConfChangeConfiguration *conf);
bool jsonPackConfClearCache(RpcPacket *rpcPacket, ConfClearCache *conf);
bool jsonPackConfDataTransfer(RpcPacket *rpcPacket, ConfDataTransfer *conf);
bool jsonPackConfGetConfiguration(RpcPacket *rpcPacket, ConfGetConfiguration *conf);
bool jsonPackConfGetLocalListVersion(RpcPacket *rpcPacket, ConfGetLocalListVersion *conf);
bool jsonPackConfRemoteStartTransaction(RpcPacket *rpcPacket, ConfRemoteStartTransaction *conf);
bool jsonPackConfRemoteStopTransaction(RpcPacket *rpcPacket, ConfRemoteStopTransaction *conf);
bool jsonPackConfReset(RpcPacket *rpcPacket, ConfReset *conf);
bool jsonPackConfUnlockConnector(RpcPacket *rpcPacket, ConfUnlockConnector *conf);

bool jsonUnpackConfAuthorize(cJSON* json, ConfAuthorize *conf);
bool jsonUnpackConfBootNotification(cJSON* json, ConfBootNotifiaction *conf);
bool jsonUnpackConfHeartbeat(cJSON* json, ConfHeartbeat *conf);
bool jsonUnpackConfStartTransaction(cJSON* json, ConfStartTransaction *conf);

//Request from Server
bool jsonUnpackReqChangeAvailability(cJSON* json, RequestChangeAvailability *req);
bool jsonUnpackReqChangeConfiguration(cJSON* json, RequestChangeConfiguration *req);
bool jsonUnpackReqGetConfiguration(cJSON* json, RequestGetConfiguration *req);
bool jsonUnpackReqRemoteStartTransaction(cJSON* json, RequestRemoteStartTransaction *req);
bool jsonUnpackReqRemoteStopTransaction(cJSON* json, RequestRemoteStopTransaction *req);
bool jsonUnpackReqReset(cJSON* json, RequestReset *req);
bool jsonUnpackReqUnlockConnector(cJSON* json, RequestUnlockConnector *req);

