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

// Convert an integer to it's binary representation 
int int_to_binary(int n)
{
	char* bin; 
	int temp;
	int i = 0; 

	for(i=31; i >= 0; i--)
	{

	}
} 

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

	printf("Assigning server port\n"); 
	servAddr.sin_port = htons(servPort); 		// Server port 
	printf("Successfully assigned server port\n"); 

	// Establish the connection to the echo server 
	if(connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
	{
		printf("connect() failed"); 
		return 0; 
	}

	// Receive a string back from the server
	unsigned int totalBytesRcvd = 0; 		// Count the total number of bytes received 
 
	while(totalBytesRcvd < expectedStringLength)
	{
		char buffer[BUFSIZE]; 

		// Receive up to BUFSIZE - 1 to leave space for a null terminator bytes from the sender 
		ssize_t numBytes = recv(sock, buffer, BUFSIZE - 1, 0);

		printf("\nReceived %zu bytes from server...", numBytes); 

		// Not totally sure if this is working... How do we compare ssize_t numBytes?
		if(numBytes == 0)
		{
			printf("\nServer stopped sending data..."); 
			break; 
		}

		if(numBytes < 0)
			DieWithSystemMessage("recv() failed"); 
		else if(numBytes == 0)
			DieWithUserMessage("recv()", "connection closed prematurely"); 

		totalBytesRcvd += numBytes; 			// Keep a tally of total bytes
		buffer[numBytes] = '\0'; 

		if(strcmp(buffer, "Welcome to The Server\n") != 0)
		{
			// We received something different than what we were expecting 
			close(sock); 								// Close the connection
			printf("\n\nServer response unrecognized. Closing connection...\n"); 	// Print an error message 
			return 0; 

		}

		//fputs(buffer, stdout); 					// Print the echo buffer 

		printf("%s", buffer); 

		memset(&buffer[0], 0, BUFSIZE); 
	}
	
	fflush(stdout); 

	// Prompt the user to enter an 8-digit ID number and name (max 20 characters) from stdin
	char id_number[9]; 
	char name[20]; 
	int n = 0; 

	memset(&name[0], 0, 20); 
	
	//memset(&id_number[0], 0, sizeof(id_number)); 

	printf("\nEnter ID: ");

	while((ch = getchar()) != '\n' && n < 8)
	{
		id_number[n] = ch; 
		n++; 
	}

	n = 0; 

	printf("\nEnter name: "); 

	while((ch = getchar()) != '\n' && n < 20)
	{
		name[n] = ch; 
		n++; 
	}
	
	n = 0; 


	// Send the ID number and name to the server as two newline terminated strings 
	while(id_number[n]) n++; 		// Add a newline terminator 
	id_number[n] = '\n'; 

	n = 0; 
	while(name[n]) n++; 
	name[n] = '\n'; 

	// It's a good idea to conserve space here by only sending the bytes you need. 
	// Namely, it would be good to remove the empty spaces in name
	char shortenedName[n]; 
	strncpy(shortenedName, name, n); 

	int sizeOf_id_number = sizeof(id_number); 
	int sizeOf_name = sizeof(name); 

	printf("\nid_number is %d bytes\n", sizeOf_id_number); 
	printf("\nname is %d bytes\n", sizeOf_name); 

	int repeatSendCounter = 1; 
	int successfullySent = 0; 


	while(repeatSendCounter < 4 && !successfullySent)
	{
		printf("\nSending ID number and name... Attempt #%d\n", repeatSendCounter); 
		ssize_t numBytes = send(sock, id_number, sizeOf_id_number, 0); 

		if(numBytes < 0)
			DieWithSystemMessage("send() failed\n"); 
		else if(numBytes != sizeOf_id_number)
			DieWithUserMessage("send()", "sent unexpected number of bytes"); 

		printf("Successfully sent (%d bytes) to the server... ID Number: %s \n", numBytes, id_number);  

		numBytes = send(sock, name, sizeOf_name, 0); 

		printf("numBytes = %d\n", numBytes); 

		if(numBytes < 0)
			DieWithSystemMessage("send() failed\n"); 
		else if(numBytes != sizeOf_name)
			DieWithUserMessage("send()", "sent unexpected number of bytes"); 

		printf("Successfully sent (%d bytes) to the server... Name: %s \n", numBytes, name);  

		char successBuffer[8]; 			// We can use 7 because both "Success" and "Failure" are 7 letter words 
		numBytes = recv(sock, successBuffer, 7, 0);

		if(numBytes < 0)
			DieWithSystemMessage("recv() failed\n"); 
		else if(numBytes != 7)
			DieWithUserMessage("rev()", "received unexpected number of bytes"); 

		printf("numBytes = %d\n", numBytes); 

		printf("Received message: %s", successBuffer); 

		if(strncmp(successBuffer, "Success", 7) == 0)
		{
			// The message is 'Success', indicating that the server received the ID number and the name 
			printf("\nThe server has successfully received the ID number and name."); 
			successfullySent = 1;
		}

		 repeatSendCounter++; 
	}

	char password[512]; 
	n=0; 

	if(successfullySent)
	{
		// Prompt the user for a max. 512 character password
		printf("\nEnter your password: "); 

		while((ch = getchar()) != '\n' && n < 512)
		{
			password[n] = ch; 
			n++; 
		}

		int passwordLength = strlen(password); 
		printf("\nYour password is %d characters", passwordLength); 

		// Send the length as a 2-byte binary number in network byte order
		int bin = htons(passwordLength); 
		printf("\nbin: %d", bin); 
		ssize_t numBytes = send(sock, bin, 2, 0); 

		if(numBytes < 0)
			DieWithSystemMessage("send() failed\n"); 
		else if(numBytes != sizeOf_id_number)
			DieWithUserMessage("send()", "sent unexpected number of bytes"); 

		numBytes = send(sock, password, 512, 0); 
		
		if(numBytes < 0)
			DieWithSystemMessage("send() failed\n"); 
		else if(numBytes != sizeOf_id_number)
			DieWithUserMessage("send()", "sent unexpected number of bytes"); 

		printf("\nSuccessfully sent (%d bytes) to the server... Password: %s \n", numBytes, password);  



		// Send the password 

	}






	printf("\n"); 


	// Close the socket 
	close(sock); 

	//printf("Press any key to exit..."); 
	//ch = getchar(); 
	//return 0; 
}