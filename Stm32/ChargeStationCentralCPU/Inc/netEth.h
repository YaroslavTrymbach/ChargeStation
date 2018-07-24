#ifndef _WIN32
#include <stdbool.h>
#include <stdint.h>
#else
#include "defTypes.h"
#endif

void NET_ETH_init(void);

void NET_ETH_change_local_ip(void);

bool NET_ETH_isConnected(void);
bool NET_ETH_connect(void);

void NET_ETH_setRemoteHost(char *host);
void NET_ETH_setRemotePort(int port);

int NET_ETH_send(const void *data, int size);
int NET_ETH_recv(void *data, int size);
