#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <iostream>
#include <errno.h>

using namespace std;

#define ECHOMAX 1024     /* Longest string to echo */

void DieWithError(char *errorMessage)
{
	fprintf(stderr, "Error: %s\n",errorMessage);
	exit(1);
}

ssize_t Readline(int fd, void *vptr, size_t maxlen);
ssize_t Writeline(int fc, const void *vptr, size_t maxlen);

int main(int argc, char *argv[])
{
	int sock;                        /* Socket descriptor */
	struct sockaddr_in echoServAddr; /* Echo server address */
	unsigned short echoServPort;     /* Echo server port */
	char echoBuffer[ECHOMAX+1];      /* Buffer for receiving echoed string */
	char *servIP;                    /* IP address of server */
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
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		DieWithError("socket() failed");

	/* Construct the server address structure */
	memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
	echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
	echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
	echoServAddr.sin_port   = htons(echoServPort);     /* Server port */

	/*  connect() to the remote echo server  */

	if ( connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr) ) < 0 )
		DieWithError("ECHOCLNT: Error calling connect()\n");


	char line[1024];
	char resp[1024];

	cout<<"> ";
	cin.getline(line, 1024);
	int written;

	while(cin) {
		cout<<"\tReq: ["<<line<<"]"<<endl;

		written = write(sock, line, strlen(line));

		//cerr<<"Escrito: "<<written<<endl;

		respStringLen = read(sock, echoBuffer, ECHOMAX-1);
		//Writeline(sock, line, strlen(line));
		//respStringLen = Readline(sock, echoBuffer, ECHOMAX-1);

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

ssize_t Readline(int sockd, void *vptr, size_t maxlen) {
	ssize_t n, rc;
	char    c, *buffer;

	buffer = (char*)vptr;

	for ( n = 1; n < maxlen; n++ ) {

		if ( (rc = read(sockd, &c, 1)) == 1 ) {
			*buffer++ = c;
			if ( c == '\n' )
				break;
		}
		else if ( rc == 0 ) {
			if ( n == 1 )
				return 0;
			else
				break;
		}
		else {
			if ( errno == EINTR )
				continue;
			return -1;
		}
	}

	*buffer = 0;
	return n;
}


/*  Write a line to a socket  */

ssize_t Writeline(int sockd, const void *vptr, size_t n) {
	size_t      nleft;
	ssize_t     nwritten;
	const char *buffer;

	buffer = (const char*)vptr;
	nleft  = n;

	while ( nleft > 0 ) {
		if ( (nwritten = write(sockd, buffer, nleft)) <= 0 ) {
			if ( errno == EINTR )
				nwritten = 0;
			else
				return -1;
		}
		nleft  -= nwritten;
		buffer += nwritten;
	}

	return n;
}

