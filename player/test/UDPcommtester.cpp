#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <iostream>

using namespace std;

#define ECHOMAX 1024     /* Longest string to echo */

void DieWithError(char *errorMessage)
{
	fprintf(stderr, "Error: %s\n",errorMessage);
	exit(1);
}

int main(int argc, char *argv[])
{
	int sock;                        /* Socket descriptor */
	struct sockaddr_in echoServAddr; /* Echo server address */
	struct sockaddr_in fromAddr;     /* Source address of echo */
	unsigned short echoServPort;     /* Echo server port */
	unsigned int fromSize;           /* In-out of address size for recvfrom() */
	char *servIP;                    /* IP address of server */
	char echoBuffer[ECHOMAX+1];      /* Buffer for receiving echoed string */
	int respStringLen;               /* Length of received response */

	if ((argc < 2) || (argc > 3))    /* Test for correct number of arguments */
	{
		fprintf(stderr,"Usage: %s <Server IP>  [<Echo Port>]\n", argv[0]);
		exit(1);
	}

	servIP = argv[1];           /* First arg: server IP address (dotted quad) */

	if (argc == 3)
		echoServPort = atoi(argv[2]);  /* Use given port, if any */
	else
		echoServPort = 7;  /* 7 is the well-known port for the echo service */

	/* Create a datagram/UDP socket */
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		DieWithError("socket() failed");

	/* Construct the server address structure */
	memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
	echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
	echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
	echoServAddr.sin_port   = htons(echoServPort);     /* Server port */

	char line[1024];
	char resp[1024];

	cout<<"> ";
	cin.getline(line, 1024);

	while(cin) {
		cout<<"\tReq: ["<<line<<"]"<<endl;

		sendto(sock, line, strlen(line), 0, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr));
		fromSize = sizeof(fromAddr);
		respStringLen = recvfrom(sock, echoBuffer, ECHOMAX, 0,
					     (struct sockaddr *) &fromAddr, &fromSize);

		strncpy(resp, echoBuffer, respStringLen);
		resp[respStringLen] = '\0';
		cout<<"\tRep: ["<<resp<<"]"<<endl;

		cout<<"> ";
		cin.getline(line, 1024);
	};
	cout<<"exiting"<<endl;



	close(sock);
	exit(0);
}
