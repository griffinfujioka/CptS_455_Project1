#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "DieWithMessage.c"

enum sizeConstants {
  MAXSTRINGLENGTH = 128,
  BUFSIZE = 512,
};

int main(int argc, char* argv[])
{
	char ch; 
	int i = 0; 
	char* servIP; 
	in_port_t servPort; 

	int expectedStringLength = strlen("Welcome to The Server\n"); 


	printf("Hello, welcome to the CLIENT PROGRAM!\n\n"); 

	printf("%d arguments\n", argc); 

	// Check for the correct number of arguments
	if(argc < 3 || argc > 4)
	{
		printf("Invalid number of arguments!"); 
		DieWithUserMessage("Parameters", "<Server Address> <Port #>"); 
		return 0; 
	}
		

	// argv[1] = Server name or IP address
	if(argv[1] != 0)
	{
		printf("Server name or IP address : %s\n", argv[1]); 
		servIP = argv[1]; 
	}
		
	// argv[2] = Port # 
	if(argv[2] != 0)
	{
		printf("Port # : %s\n", argv[2]); 
		servPort = atoi(argv[2]); 
	}
		

	// Create a reliable, stream socket using TCP; use the socket() syscall 
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 		// int socket(address_family, type, protocol)

	if(sock < 0)
	{
		printf("socket() failed"); 
		return 0; 
	}

	// Construct the server address structure
	struct sockaddr_in servAddr; 				// Server address
	memset(&servAddr, 0, sizeof(servAddr)); 	// Zero out the struct 
	servAddr.sin_family = AF_INET; 				// IPv4 address family 

	// Convert address
	int rtnVal = inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr); 

	if(rtnVal == 0)
	{
		printf("inet_pton() failed. Invalid address string");
		return 0;
	}
	else if(rtnVal < 0)
	{
		printf("inet_pton() failed"); 
		return 0; 
	}

	servAddr.sin_port = htons(servPort); 		// Server port 

	// Establish the connection to the echo server 
	if(connect(sock, (struct sockaddr *) & servAddr, sizeof(servAddr)) < 0)
	{
		printf("connect() failed"); 
		return 0; 
	}

	// Receive a string back from the server
	unsigned int totalBytesRcvd = 0; 		// Count the total number of bytes received 
	fputs("Received: ", stdout); 
	while(totalBytesRcvd < expectedStringLength)
	{
		char buffer[BUFSIZE]; 

		// Receive up to BUFSIZE - 1 to leave space for a null terminator bytes from the sender 
		ssize_t numBytes = recv(sock, buffer, BUFSIZE - 1, 0);

		if(numBytes < 0)
			DieWithSystemMessage("recv() failed"); 
		else if(numBytes == 0)
			DieWithUserMessage("recv()", "connection closed prematurely"); 

		totalBytesRcvd += numBytes; 			// Keep a tally of total bytes
		buffer[numBytes] = '\0'; 

		fputs(buffer, stdout); 					// Print the echo buffer 
	}


	// Close the socket 
	close(sock); 

	printf("Press any key to exit..."); 
	ch = getchar(); 
	return 0; 
}