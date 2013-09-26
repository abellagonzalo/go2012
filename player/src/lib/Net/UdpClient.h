#ifndef UDPClient_H
#define UDPClient_H

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <iostream>

class UdpClient
{
public:

	UdpClient();
	~UdpClient();

	void initUDPConn(int port, const char *ip);
	void sendUDP(char *data, int size);

private:

    int sock;                        /* Socket descriptor */
    struct sockaddr_in ServAddr; /* Echo server address */
};

#endif

