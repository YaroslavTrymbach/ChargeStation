#include "netEth.h"
#include "lwip.h"
#include "lwip/sockets.h"
#include "lwip/dhcp.h"
#include "settings.h"

#include <string.h>

extern struct netif gnetif;
int remotePort;
char remoteHost[64];
bool isConnected = false;
int e_sock;

void NET_ETH_init(void){
	MX_LWIP_InitMod();
	
	e_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

void NET_ETH_change_local_ip(void){
	ChargePointSetting* st;
	st = Settings_get();
	netif_set_down(&gnetif);

	if(st->isDHCPEnabled){
		memset(&gnetif.ip_addr, 0x00, 4);
		memset(&gnetif.netmask, 0x00, 4);
		memset(&gnetif.gw, 0x00, 4);
	}
	else{
		memcpy(&gnetif.ip_addr, st->LocalIp, 4);
		memcpy(&gnetif.netmask, st->NetMask, 4);
		memcpy(&gnetif.gw, st->GetewayIp, 4);
	}
	
	netif_set_up(&gnetif);
	
	if(st->isDHCPEnabled){
		dhcp_start(&gnetif);
	}
}

bool NET_ETH_isConnected(void){
	return isConnected;
}

bool NET_ETH_connect(void){
	struct sockaddr_in server;
	
	server.sin_addr.s_addr = inet_addr(remoteHost);
  server.sin_family = AF_INET;
  server.sin_port = htons(remotePort);
	
	if (connect(e_sock , (struct sockaddr *)&server , sizeof(server)) < 0){
       return false;
  }
	
	isConnected = true;
	return true;
}

void NET_ETH_setRemoteHost(char *host){
	strcpy(remoteHost, host);
}

void NET_ETH_setRemotePort(int port){
	remotePort = port;
}

int NET_ETH_send(const void *data, int size){
	int res;
	
	if(!isConnected)
		return -1;
	
	res = send(e_sock, data, size, 0);
	
	if(res == -1){
		if(errno == ECONNRESET){
			isConnected = false;
		}
	}
	
	return res;
}
