#ifndef UDPServer_H
#define UDPServer_H

#include <pthread.h>
#include <sys/wait.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "../Common.h"


class UdpServer
{
public:

	UdpServer();
	~UdpServer();

	void initUDPConn(int selectedport);
	virtual void UDPmanageRecv(int size) = 0;
	void sendUDP(int size);

	int getUDPPort() { return port;};
	int getUDPSocket() { return list_sock;};
	char *getUDPBuffer() { return buf;};
	sockaddr_in *getUDPRespSock() { return &si_other;};

private:

	static const int MAXMSGSIZE = 640*480*3+128;

	char buf[MAXMSGSIZE];

    static void* startThread(void *obj);
    pthread_t tserver;
    
	int list_sock;
	int port;
	sockaddr_in si_other;

	struct sockaddr_in servaddr;   
};

#endif

