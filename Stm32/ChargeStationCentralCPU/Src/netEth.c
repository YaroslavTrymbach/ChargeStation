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
bool isLinkUpDone = false;
int e_sock = - 1;

bool NET_ETH_check_link_up();

void doLinkUp(){
	ChargePointSetting *settings;
	
	settings = Settings_get();
	
	//netif_set_up(&gnetif);
	
	if (netif_is_link_up(&gnetif))
  {
    // When the netif is fully configured this function must be called 
    netif_set_up(&gnetif);
		isLinkUpDone = true;
  }
  else
  {
    // When the netif link is down this function must be called 
    netif_set_down(&gnetif);
  }

	if(settings->isDHCPEnabled)
		dhcp_start(&gnetif);
	
	//isLinkUpDone = true;
}

void removeNetEthIntf(){
}

void NET_ETH_close(void){
	close(e_sock);
	e_sock = -1;
	isConnected = false;
}

void NET_ETH_init(void){	
	MX_LWIP_InitMod();
	if(NET_ETH_check_link_up()){
		doLinkUp();
	}
	
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
	struct timeval tv;
	int opt, res;
	
	if(!NET_ETH_check_link_up())
		return false;
	
	if(!isLinkUpDone)
		doLinkUp();
	
	if(e_sock == -1){
		e_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(e_sock == -1)
			return false;
	}
	
	
	
	server.sin_addr.s_addr = inet_addr(remoteHost);
  server.sin_family = AF_INET;
  server.sin_port = htons(remotePort);
	
	if (connect(e_sock , (struct sockaddr *)&server , sizeof(server)) < 0){
			NET_ETH_close();
      return false;
  }
	
	opt = 100; //Time out in milliseconds
	tv.tv_sec = opt / 1000;
	tv.tv_usec = (opt % 1000) * 1000;
	res = setsockopt(e_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	if(res == -1){
		printf("setsockopt failed. Err = %d %s\n", errno, strerror(errno));
		if(errno == EINVAL){
			printf("INVAL\n");
		}
	}
	
	isConnected = true;
	return true;
}

bool NET_ETH_disconnect(void){
	if(isConnected)
		NET_ETH_close();
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
			NET_ETH_close();
		}
	}
	
	return res;
}

int NET_ETH_recv(void *data, int size){
	int res;
	
	if(!isConnected)
		return -1;
	
	res = recv(e_sock, data, size, 0);
	
	if(res == -1){
		if(errno == EAGAIN){
			//This timeout
			res = 0;
		}
		if(errno == ECONNRESET){
			NET_ETH_close();
		}
	}
	
	return res;
}

bool NET_ETH_check_link_up(){
	uint32_t phyreg;

	HAL_ETH_ReadPHYRegister(&heth, PHY_BSR, &phyreg);
	return ((phyreg & PHY_LINKED_STATUS) != 0);
}