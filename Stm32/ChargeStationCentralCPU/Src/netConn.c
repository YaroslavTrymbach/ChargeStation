#include "netConn.h"
#include "netEth.h"

int connType = CONN_TYPE_ETH;

void NET_CONN_init(void){
	switch(connType){
		case CONN_TYPE_ETH:
			NET_ETH_init();
			break;
	}
}

void NET_CONN_change_local_ip(void){
	switch(connType){
		case CONN_TYPE_ETH:
			NET_ETH_change_local_ip();
			break;
	}
}