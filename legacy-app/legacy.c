#include <stdio.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

void commandline(char *argv[])
{
	fprintf(stderr, "Usage: %s <Port>\n", argv[0]);
	exit(1);
}

void error(char *error, ...)
{
	va_list argp;
//	fprintf(stderr, "Error: %s\n", error);
//	fprintf(stderr, "%s, line %d: error:", filename, lineno);
	va_start(argp, error);
	vfprintf(stderr, error, argp);
	va_end(argp);
	fprintf(stderr, "\n");
	exit(1);
}

int main(int argc, char *argv[])
{
	int serverport = 0,serversocket=-1,clientsocket=-1;
	unsigned int clientlen=0; //serverlen=0;
	struct sockaddr_in serveraddress;
	struct sockaddr_in clientaddress;
	static char * bannerfmt = "Hello %s, it's very nice to meet you.\n";
	char banner[512] = "\000";

	memset(&serveraddress, 0x00, sizeof(serveraddress));
	memset(&clientaddress, 0x00, sizeof(clientaddress));

	if (argc != 2)
	{
		commandline(argv);
	}

	serverport = atoi(argv[1]);
	if (errno != 0)
	{
		commandline(argv);
	}

	//Create socket
	serversocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serversocket < 0 || errno != 0)
	{
		error("Unable to establish TCP Socket for Server.  Errno: %s", strerror(errno));
	}

	//Construct local address
	serveraddress.sin_family = AF_INET;
	serveraddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddress.sin_port = htons(serverport);

	//Bind
	if (bind(serversocket, (struct sockaddr *) &serveraddress, sizeof(serveraddress)) < 0)
	{
		error("Opening the TCP server socket failed on this host. Error: %s", strerror(errno));
	}


	//Listen
	if (listen(serversocket, 1) < 0)
	{
		error("Unable to mark the socket to listen for connections. Error: %s", strerror(errno));
	}

	while(1)
	{
	clientlen = sizeof(clientaddress);
	
	//Wait for a client to connect to an accept a socket.
	if ((clientsocket = accept(serversocket, (struct sockaddr *) &clientaddress, &clientlen)) < 0)
	{
		error("Socket ACCEPT failed.  clientsocket = %d, serversocket = %d, errno = %d, strerror = %s", clientsocket, serversocket, errno, strerror(errno));
	}
	
	fprintf(stdout, "Handling client %s\n", inet_ntoa(clientaddress.sin_addr));
	snprintf(banner, (size_t) sizeof(banner) - 1, bannerfmt, inet_ntoa(clientaddress.sin_addr));
	
	send(clientsocket, banner, strlen(banner), 0);
	close(clientsocket);
	}	
return (0);
}
