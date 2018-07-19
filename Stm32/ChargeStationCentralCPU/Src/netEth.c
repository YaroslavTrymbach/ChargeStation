#include "netEth.h"
#include "lwip.h"
#include "lwip/dhcp.h"
#include "settings.h"

#include <string.h>

extern struct netif gnetif;

void NET_ETH_init(void){
	MX_LWIP_InitMod();
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
