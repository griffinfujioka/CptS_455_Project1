#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "DieWithMessage.c"

enum sizeConstants {
  MAXSTRINGLENGTH = 128,
  BUFSIZE = 512,
};

static const int MAXPENDING = 5; 		// Maximum outstanding connection requests 

void HandleTCPClient(int clntSocket); 

int main(int argc, char* argv[])
{
	char ch; 
	int i = 0; 

	printf("Hello, welcome to the SERVER PROGRAM!\n\n"); 

	/*printf("%d arguments provided\nArguments: \n", argc); 

	for(i=0; i<argc; i++)
	{
		printf("\targv[%d] : %s\n", i, argv[i]); 
	}*/ 

	if(argc != 2)
		DieWithUserMessage("Parameters:", "<Server Port>"); 

	in_port_t servPort = atoi(argv[1]); 		// local port 

	// Create socket for incoming connections 
	int servSock; 		// Socket descriptor 

	if((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		DieWithSystemMessage("socket() failed"); 

	// Construct local address structure 
	struct sockaddr_in servAddr; 			// local address
	memset(&servAddr, 0, sizeof(servAddr)); 		// zero out structure 
	servAddr.sin_family = AF_INET;				// IPV4 address family 
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); 		// any incoming interface 
	servAddr.sin_port = htons(servPort); 		// local port 

	// Bind to the local address 
	if(bind(servSock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0)
		DieWithSystemMessage("bind() failed"); 

	// Mark the socket so it will list for incoming connections 
	if(listen(servSock, MAXPENDING) < 0)
		DieWithSystemMessage("listen() failed"); 


	for(;;)			// Run forever 
	{
		struct sockaddr_in clntAddr; 		// client address

		// Set length of client address structure (in-out parameter)
		socklen_t clntAddrLen = sizeof(clntAddr);

		// Wait for a client to connect
		int clntSock = accept(servSock, (struct sockaddr*) &clntAddr, &clntAddrLen); 

		if(clntSock < 0)
			DieWithSystemMessage("accept() failed"); 

		// clntSock is connected to a client! 

		char clntName[INET_ADDRSTRLEN]; 		// String for client address
		if(inet_ntop(AF_INET, &clntAddr.sin_addr.s_addr, clntName, sizeof(clntName)) != NULL)
		{
			printf("Handling client %s/%d\n", clntName, ntohs(clntAddr.sin_port));


			// Send a welcome message to the client 
			char* welcomeMessage = "Welcome to The Server Griffin\n";

			printf("Attempting to send a welcome message...");
			send(clntSock, welcomeMessage, strlen(welcomeMessage), 0); 
			printf("Successfully sent a welcome message."); 
		}		 
		else
			printf("Unable to get client address"); 

		


	}

	if(argv[1] != 0)
		printf("Port # on which to listen : %s\n", argv[1]); 





	printf("Press any key to exit..."); 
	ch = getchar(); 
	return 0; 
}

void HandleTCPClient(int clntSocket)
{
	char buffer [BUFSIZE]; 		// Buffer for string

}