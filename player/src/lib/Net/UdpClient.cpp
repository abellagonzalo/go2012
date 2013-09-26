#include "UdpClient.h"

UdpClient::UdpClient()
{

}

UdpClient::~UdpClient()
{

}

void
UdpClient::initUDPConn(int port, const char *ip)
{
    /* Create a datagram/UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
    	std::cerr<<"UdpClient::initUDPConn Error to create socket"<<std::endl;
    	return;
    }

    /* Construct the server address structure */
    memset(&ServAddr, 0, sizeof(ServAddr));    /* Zero out structure */
    ServAddr.sin_family = AF_INET;                 /* Internet addr family */
    ServAddr.sin_addr.s_addr = inet_addr(ip);  /* Server IP address */
    ServAddr.sin_port   = htons(port);     /* Server port */
}


void
UdpClient::sendUDP(char *data, int size)
{
    if(sendto(sock, data, size, 0, (struct sockaddr *)
               &ServAddr, sizeof(ServAddr)) < 0)
    {
    	std::cerr<<"UdpClient::sendUDP Error to create socket"<<std::endl;
    	return;
    }

}
