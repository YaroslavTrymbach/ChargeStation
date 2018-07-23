#include "ocppConfigurationDef.h"

#define DEF_AUTHORIZE_REMOTE_TX_REQUEST    true
#define DEF_AUTHORIZE_REMOTE_TX_REQUEST_RO true

void fillOcppConfigurationWithDefValues(OcppConfigurationVaried *confVaried,
										OcppConfigurationFixed *confFixed,
										OcppConfigurationRestrict *confRestrict){

	//AuthorizeRemoteTxRequest
	confRestrict->authorizeRemoteTxRequestsReadOnly = DEF_AUTHORIZE_REMOTE_TX_REQUEST_RO;
	confVaried->authorizeRemoteTxRequests = DEF_AUTHORIZE_REMOTE_TX_REQUEST;
	confFixed->authorizeRemoteTxRequests = DEF_AUTHORIZE_REMOTE_TX_REQUEST;
}