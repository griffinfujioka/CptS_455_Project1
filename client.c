#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char* argv[])
{
	char ch; 
	int i = 0; 
	char* servIP; 
	in_port_t servPort; 

	printf("Hello, welcome to the CLIENT PROGRAM!\n\n"); 

	printf("%d arguments\n", argc); 

	// Check for the correct number of arguments
	if(argc < 3 || argc > 4)
	{
		printf("Invalid number of arguments!"); 
		return 0; 
	}
		

	if(argv[1] != 0)
	{
		printf("Server name or IP address : %s\n", argv[1]); 
		servIP = argv[1]; 
	}
		

	if(argv[2] != 0)
	{
		printf("Port # : %s\n", argv[2]); 
		servPort = atoi(argv[2]); 
	}
		

	// Create a reliable, stream socket using TCP 
	
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



	printf("Press any key to exit..."); 
	ch = getchar(); 
	return 0; 
}