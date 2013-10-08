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

	// The response we're expecting from the server 
	int expectedStringLength = strlen("Welcome to The Server\n"); 			


	printf("Hello, welcome to the CLIENT PROGRAM!\n\n"); 

	//printf("%d arguments\n", argc); 

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

		printf("\nServer said: %s", buffer); 

		memset(&buffer[0], 0, BUFSIZE); 
	}
	
	fflush(stdout); 

	// Prompt the user to enter an 8-digit ID number and name (max 20 characters) from stdin
	char id_number[9]; 
	char name[20]; 
	int n = 0; 

	memset(&name[0], 0, 20); 
	

	printf("\nEnter ID: ");

	while((ch = getchar()) != '\n' && n < 8)
	{
		id_number[n] = ch; 
		n++; 
	}

	n = 0; 

	printf("Enter name: "); 

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
	// Namely, it would be good to remove the empty spaces in name buffer
	char shortenedName[n]; 
	strncpy(shortenedName, name, n); 

	int sizeOf_id_number = sizeof(id_number); 
	int sizeOf_name = sizeof(name); 

	printf("\nid_number is %d bytes", sizeOf_id_number); 
	printf("\nname is %d bytes\n", sizeOf_name); 

	int repeatSendCounter = 1; 
	int successfullySent = 0; 


	while(repeatSendCounter < 4 && !successfullySent)
	{
		printf("\nAttempt #%d", repeatSendCounter);
		printf("\nSending ID number and name..."); 
		ssize_t numBytes = send(sock, id_number, sizeOf_id_number, 0); 

		if(numBytes < 0)
			DieWithSystemMessage("send() failed\n"); 
		else if(numBytes != sizeOf_id_number)
			DieWithUserMessage("send()", "sent unexpected number of bytes"); 

		printf("\nSuccessfully sent (%zu bytes) to the server... ID Number: %s \n", numBytes, id_number);  

		numBytes = send(sock, name, sizeOf_name, 0); 


		if(numBytes < 0)
			DieWithSystemMessage("send() failed\n"); 
		else if(numBytes != sizeOf_name)
			DieWithUserMessage("send()", "sent unexpected number of bytes"); 

		printf("Successfully sent (%zu bytes) to the server... Name: %s \n", numBytes, name);  

		char successBuffer[8]; 			// We can use 7 because both "Success" and "Failure" are 7 letter words 
		numBytes = recv(sock, successBuffer, 7, 0);

		if(numBytes < 0)
			DieWithSystemMessage("recv() failed\n"); 
		else if(numBytes != 7)
			DieWithUserMessage("rev()", "received unexpected number of bytes");  

		printf("Received message: %s", successBuffer); 

		if(strncmp(successBuffer, "Success", 7) == 0)
		{
			// The message is 'Success', indicating that the server received the ID number and the name 
			printf("\nThe server has successfully received the ID number and name."); 
			successfullySent = 1;
		}
		repeatSendCounter++;

		if(strncmp(successBuffer, "Failure", 7) == 0 && repeatSendCounter > 3)
		{
			// The message is 'Failure', indicating that the server did not find the ID number and name 
			printf("\nServer failed to find ID number and name combination. Server has closed the socket"); 
			close(sock); 
		} 
	}

	char password[512]; 
	n=0; 

	if(successfullySent)
	{
		// We've successfully sent the ID number and name
		// Prompt the user for a max. 512 character password
		int validPassword = 0; 
		int numFailures = 0; 


		while(!validPassword && numFailures < 3)
		{
			
			memset(password, 0, 512); 		// Reset the password buffer 
			n=0; 

			
			printf("\nEnter your password: "); 


			while((ch = getchar()) != '\n' && n < 512)
			{
				password[n] = ch; 
				n++; 
			}

			password[n] = '\0'; 

			printf("Password length: %d", n); 

			printf("\nPassword: %s", password); 

			u_short passwordLength = n; 

			uint32_t network_byte_order = htons(passwordLength); 
			printf("\nnetwork_byte_order = %d", network_byte_order); 

			int host_byte_order = ntohs(network_byte_order); 

			printf("\nhost_byte_order = %d", host_byte_order); 

			if(host_byte_order == passwordLength)
				printf("... Which is the password length!"); 

			//Send the length as a 2-byte binary number in network byte order
			ssize_t numBytes = send(sock, &network_byte_order, sizeof(network_byte_order), 0); 

			if(numBytes < 0)
				DieWithSystemMessage("send() failed\n"); 
			else if(numBytes != sizeof(network_byte_order))
				DieWithUserMessage("send()", "sent unexpected number of bytes"); 

			printf("\nSuccessfully sent (%zu bytes) to the server... Password length: %d \n", numBytes, passwordLength);  


			numBytes = send(sock, password, passwordLength, 0); 

			if(numBytes < 0)
				DieWithSystemMessage("send() failed\n"); 
			else if(numBytes != passwordLength)
				DieWithUserMessage("send()", "sent unexpected number of bytes"); 

			printf("\nSuccessfully sent (%zu bytes) to the server... Password: %s \n", numBytes, password); 

			// Receive a success or invalid message from the server. 
			char passwordValidationBuffer[BUFSIZE]; 
			numBytes = recv(sock, passwordValidationBuffer, BUFSIZE, 0); 

			printf("\nReceived %zu bytes from the server: %s", numBytes, passwordValidationBuffer); 

			if(numBytes < 0)
				DieWithSystemMessage("recv() failed\n"); 
			else if(numBytes == 256)
			{
				// This indicates a success message from the server
				validPassword = 1; 
				printf("\nValid password! Closing the socket.");
				close(sock); 
			}
			else if(numBytes == 128)
			{
				// This indicates an invalid password from the server
				numFailures++; 
				memset(&password[0], 0, 512); 
				printf("\nInvalid password. Please try again: "); 
			}
		}

		if(numFailures == 3)
		{
			// Our password has failed 3 times 
			printf("\nInvalid password occurred 3 times... Closing the socket.");
			close(sock); 
		}
		 

	}







	printf("\n"); 


	// Close the socket 
	close(sock); 

	//printf("Press any key to exit..."); 
	//ch = getchar(); 
	return 0; 
}