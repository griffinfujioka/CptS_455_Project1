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

	printf("Hello, welcome to the CLIENT PROGRAM!\n\n"); 

	printf("%d arguments", argc); 

	// Check for the correct number of arguments
	if(argc < 3 || argc > 4)
	{
		printf("Invalid number of arguments!"); 
		return 0; 
	}
		

	if(argv[1] != 0)
		printf("Server name or IP address : %s\n", argv[1]); 

	if(argv[2] != 0)
		printf("Port # : %s\n", argv[2]); 

	// Create a reliable, stream socket using TCP 
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 

	if(sock < 0)
	{
		printf("socket() failed"); 
		return 0; 
	}



	printf("Press any key to exit..."); 
	ch = getchar(); 
	return 0; 
}