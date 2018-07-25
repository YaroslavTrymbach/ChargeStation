#ifndef _WIN32
#include <stdbool.h>
#include <stdint.h>
#else
#include "defTypes.h"
#endif

#define CONN_TYPE_ETH  0
#define CONN_TYPE_GSM  1
#define CONN_TYPE_WIFI 2

void NET_CONN_init(void);

void NET_CONN_change_local_ip(void);

bool NET_CONN_isConnected(void);
bool NET_CONN_connect(void);
bool NET_CONN_disconnect(void);

void NET_CONN_setRemoteHost(char *host);
void NET_CONN_setRemotePort(int port);

int NET_CONN_send(const void *data, int size);
int NET_CONN_recv(void *data, int size);
	