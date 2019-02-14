#include "ocppConfigurationDef.h"

#define DEF_AUTHORIZE_REMOTE_TX_REQUEST    true
#define DEF_AUTHORIZE_REMOTE_TX_REQUEST_RO true

#define DEF_CONNECTION_TIME_OUT 600
	
#define DEF_LOCAL_AUTHORIZE_OFFLINE true
#define DEF_LOCAL_PRE_AUTHORIZE     true

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
}