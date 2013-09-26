#include "UdpServer.h"

using namespace std;

UdpServer::UdpServer()
{

}

UdpServer::~UdpServer()
{
	close(list_sock);
}

void UdpServer::initUDPConn(int selectedport)
{
	port = selectedport;
	
	if((list_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
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
		cerr << begin_red << "UDPSERV: Error calling bind()" << end_color << endl;
		return;
	}

	if ( bind(list_sock, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) 
	{
		cerr << begin_red << "UDPSERV: Error calling bind()" << end_color << endl;
		perror("Descr: ");
		return;
	}else
	{
		//cerr << "UDPSERV: bind() to port" << port << endl;
	}

	pthread_create(&tserver, NULL, UdpServer::startThread, this);
}

void* UdpServer::startThread(void *obj)
{
	UdpServer *instance = reinterpret_cast<UdpServer *>(obj);

	socklen_t londesde;
	londesde = sizeof(sockaddr_in);

	while(true)
	{
		int size;
		londesde = sizeof(sockaddr_in);

		(instance->getUDPRespSock())->sin_port = htons (instance->getUDPPort());

		//cerr<<"UDPSERV: listening"<<endl;

		if ( (size = recvfrom(instance->getUDPSocket(), instance->getUDPBuffer(), MAXMSGSIZE, 0, (sockaddr *)instance->getUDPRespSock(), &londesde))  < 0 ) {
			cerr<<"UDPSERV: Error calling recvfrom()"<<endl;
			return NULL;
		}

		if(size>0)
			instance->UDPmanageRecv(size);
	}
	
}


/*void
UdpServer::UDPmanageRecv(int size)
{

	send(size);
}*/

void 
UdpServer::sendUDP(int size)
{
	socklen_t londesde;
	londesde = sizeof(struct sockaddr_in);
	
	if(sendto(list_sock, buf, size, 0, (sockaddr*)&si_other, londesde) == -1)
	{
		cerr<<"UDPSERV: Error calling sendto()"<<endl;
		return;
	}

	return;
}
