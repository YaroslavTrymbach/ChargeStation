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

bool NET_CONN_isConnected(void){
	switch(connType){
		case CONN_TYPE_ETH:
			return NET_ETH_isConnected();
	}
	return false;
}

bool NET_CONN_connect(void){
	switch(connType){
		case CONN_TYPE_ETH:
			return NET_ETH_connect();
	}
	return false;
}

bool NET_CONN_disconnect(void){
	switch(connType){
		case CONN_TYPE_ETH:
			return NET_ETH_disconnect();
	}
	return false;
}

void NET_CONN_setRemoteHost(char *host){
	switch(connType){
		case CONN_TYPE_ETH:
			NET_ETH_setRemoteHost(host);
			break;
	}
}

void NET_CONN_setRemotePort(int port){
	switch(connType){
		case CONN_TYPE_ETH:
			NET_ETH_setRemotePort(port);
			break;
	}
}

int NET_CONN_send(const void *data, int size){
	switch(connType){
		case CONN_TYPE_ETH:
			return NET_ETH_send(data, size);
	}
	return 0;
}

int NET_CONN_recv(void *data, int size){
	switch(connType){
		case CONN_TYPE_ETH:
			return NET_ETH_recv(data, size);
	}
	return 0;
}
