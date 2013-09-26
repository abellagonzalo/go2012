#include "TcpServer.h"
#include <errno.h>
#include <string.h>

using namespace std;

int TcpServer::sizetoread = 0;
char TcpServer::tempbuf[MAXMSGSIZE];
int TcpServer::mode = NORMALMODE;

TcpServer::TcpServer()
{
	sizetoread = 0;
	mode = NORMALMODE;
} 

TcpServer::~TcpServer()
{
	close(list_sock);
}

void TcpServer::initTCPConn(int port)
{
	if((list_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
	{
		cerr<<"Error creating connection"<<endl;
		return;
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(port);

	int optval=1;

	if(setsockopt(list_sock,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval))==-1)
	{
		cerr<< "TCPSERV: Error calling setsockopt()" <<endl;
		return;
	}


	if ( bind(list_sock, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) 
	{
		cerr<< "TCPSERV: Error calling bind()"<<strerror(errno)<<endl;
		return;
	}

	if ( listen(list_sock, MAX_CONNS) < 0 ) 
	{
		cerr<< "ECHechoBufferOSERV: Error calling listen() "<<endl;
		return;
	}

	pthread_create(&tserver, NULL, serverThread, this);

}

void* TcpServer::serverThread(void *obj)
{
	TcpServer *instance = reinterpret_cast<TcpServer *>(obj);

	while(true)
	{

		TCPconnection *connaux = new TCPconnection;

		connaux->server = instance;

		connaux->socket = accept(instance->getTCPListSocket(), NULL, NULL);

		if ( connaux->socket < 0 ) {
			cerr<<"TCPSERV: Error calling accept()"<<endl;
			delete connaux;
			return NULL;
		}

		connaux->buffer = new char[MAXMSGSIZE];

		pthread_t newt;

		pthread_create(&newt, NULL, connThread, connaux);
	}
}

void* 
TcpServer::connThread(void *obj)
{
	TCPconnection *data = reinterpret_cast<TCPconnection *>(obj);

	TcpServer *instance = data->server;

	int readed, readcount=0;

	do
	{
		if (mode == NORMALMODE)
		{
			readed = read(data->socket, data->buffer, MAXMSGSIZE);
			if (readed > 0)
				instance->TCPmanageRecv(*data, readed);
		}else
		{
			readed = read(data->socket, data->buffer, sizetoread);
			if (readed > 0)
			{
				//ToDo: Revisar esto, readcount está sin inicializar
				memcpy(&(tempbuf[readcount]), data->buffer, readed);
				readcount += readed;
				sizetoread = sizetoread - readed;
				if (sizetoread == 0)
				{
					mode = NORMALMODE;
				}
			}
		}

	}while (readed > 0);

	if ( close(data->socket) < 0 ) {
		cerr<<"TCPSERV: Error calling close()"<<endl;
	}

	delete data;
	return NULL;
}

void
TcpServer::setMode(int newmode, int size)
{
	mode = newmode;
	sizetoread = size;
}

void 
TcpServer::sendTCP(TCPconnection conn, int size)
{
	int written;
	//cerr<<"TcpServer::send\tenvio: "<<size<<" bytes"<<endl;
	written = write(conn.socket, conn.buffer, size);
}
