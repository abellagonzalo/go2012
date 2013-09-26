#ifndef TCPServer_H
#define TCPServer_H

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

#define MAX_CONNS	5
class TcpServer;



typedef struct
{
	TcpServer *server;
	int socket;
	char *buffer;
}TCPconnection;

class TcpServer
{
public:

	TcpServer();
	~TcpServer();

	void initTCPConn(int selectedport);
	virtual void TCPmanageRecv(TCPconnection conn, int size) = 0;
	void sendTCP(TCPconnection conn, int size);

	int getTCPListSocket() { return list_sock;};
	void setMode(int newmode, int size);
	char *getTempBuf() { return tempbuf; };

	static const int NORMALMODE = 0;
	static const int BIGMODE = 1;

private:
	static const int MAXMSGSIZE = 320*240*3+128+1024;
	
    static void* serverThread(void *obj);
    static void* connThread(void *obj);

	int list_sock;

	static int sizetoread;
	static char tempbuf[MAXMSGSIZE];
	static int mode;

	struct sockaddr_in servaddr;  
	
	pthread_t tconn[MAX_CONNS];
	pthread_t tserver;


};

#endif

