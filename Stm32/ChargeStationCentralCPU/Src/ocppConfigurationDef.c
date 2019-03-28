#include "ocppConfigurationDef.h"

#define DEF_AUTHORIZE_REMOTE_TX_REQUEST    true
#define DEF_AUTHORIZE_REMOTE_TX_REQUEST_RO true

#define DEF_CONNECTION_TIME_OUT 600
	
#define DEF_LOCAL_AUTHORIZE_OFFLINE true
#define DEF_LOCAL_PRE_AUTHORIZE     true

#define DEF_CLOCK_ALIGNED_DATA_INTERVAL 0

#define DEF_HEARTBEAT_INTERVAL           300
#define DEF_METER_VALUES_SAMPLE_INTERVAL 60
#define DEF_RESET_RETRIES                3

#define DEF_STOP_TRANSACTION_ON_EV_SIDE_DISCONNECT false
#define DEF_STOP_TRANSACTION_ON_INVALID_ID         false
#define DEF_UNLOCK_CONNECTOR_ON_EV_SIDE_DISCONNECT false

void fillOcppConfigurationWithDefValues(OcppConfigurationVaried *confVaried,
										OcppConfigurationFixed *confFixed,
										OcppConfigurationRestrict *confRestrict){

	//AuthorizeRemoteTxRequest
	confRestrict->authorizeRemoteTxRequestsReadOnly = DEF_AUTHORIZE_REMOTE_TX_REQUEST_RO;
	confVaried->authorizeRemoteTxRequests = DEF_AUTHORIZE_REMOTE_TX_REQUEST;
	confFixed->authorizeRemoteTxRequests = DEF_AUTHORIZE_REMOTE_TX_REQUEST;
								
	confVaried->connectionTimeOut = DEF_CONNECTION_TIME_OUT;
	confVaried->localAuthorizeOffline = DEF_LOCAL_AUTHORIZE_OFFLINE;
	confVaried->localPreAuthorize = DEF_LOCAL_PRE_AUTHORIZE;											
	confVaried->clockAlignedDataInterval = DEF_CLOCK_ALIGNED_DATA_INTERVAL;

	confVaried->heartbeatInterval = DEF_HEARTBEAT_INTERVAL;
	confVaried->meterValuesSampleInterval = DEF_METER_VALUES_SAMPLE_INTERVAL;
	confVaried->resetRetries = DEF_RESET_RETRIES;

	confVaried->stopTransactionOnEVSideDisconnect = DEF_STOP_TRANSACTION_ON_EV_SIDE_DISCONNECT;
	confVaried->stopTransactionOnInvalidId = DEF_STOP_TRANSACTION_ON_INVALID_ID;
	confVaried->unlockConnectorOnEVSideDisconnect = DEF_UNLOCK_CONNECTOR_ON_EV_SIDE_DISCONNECT;
}
									