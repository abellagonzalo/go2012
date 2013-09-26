#ifndef NetServer_H
#define NetServer_H


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


/*class NetServer
{
public:
 
	virtual void initConn(int port) = 0;
	virtual void manageRecv(char *data, int size, sockaddr *si_other) = 0;
	virtual void send(char *data, int size, sockaddr *si_other) = 0;
	
};
*/
typedef struct
{
	int socket;
	NetServer *server;
}conn;


#endif

#ifndef NetServer_H
#define NetServer_H


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

#include "../Component.h"

class NetServer
{
public:
 
	virtual void initConn(int port, void (*handler)(char*, int, Component*), Component *obj) = 0;
	virtual void send(char *data, int size) = 0;
	
protected:
	Component *objHandler;
};

typedef struct
{
	int socket;
	NetServer *server;
}conn;


#endif

