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
  DATATABLE_SIZE = 10, 
  DATATABLE_ENTRY_SIZE = 29
};

char* datatable[DATATABLE_SIZE];

void initialize_datatable()
{
	datatable[0] = "11044124 griffin\0"; 
	datatable[1] = 0; 
	datatable[2] = 0; 
	datatable[3] = 0; 
}

// Look for the provided ID number and name in the datatable array, return 1 if found, else return false. 
int LookUpIDNumberAndUsername(char id_number[8], char name[20])
{
	int i = 0; 

	if(name[0] == '\n')
	{
		printf("\nThere is a newline at the front the name"); 
		while(i < strlen(name))
		{

			name[i] = name[i+1]; 
			i++; 					// shift all the letters left by one
		}

		name[i] = '\0'; 
		name[strlen(name)] = '\0'; 
		name[strlen(name-1)] = '\0'; 
		name[strlen(name+1)] = '\0'; 

		printf("\nRemoved newline from name... See? %s blah", name); 

	}

	i = 0; 
		

	while(i < DATATABLE_SIZE)
	{

		if(datatable[i] == 0)
			return 0; 


		char currentID[8]; 
		char currentName[20]; 
		strncpy(currentID, datatable[i], 8); 

		

		printf("\n%s vs. %s", currentID, id_number); 

		if(strcmp(currentID, id_number) == 0)
		{
			// We found a matching ID number, now check if the name matches 
			int cheating = 9; 

			// This is a hack, but it's so beautiful, why fix it? 
			while(cheating < DATATABLE_ENTRY_SIZE && (datatable[i][cheating] != '\0' || datatable[i][cheating] != '\n'))
			{

				currentName[cheating - 9] = datatable[i][cheating];
				cheating++; 
			}

			currentName[cheating] = '\0'; 
			int nameLength = strlen(name); 
			int currentNameLength = strlen(currentName); 

			printf("\n%s vs. %s", currentName, name); 


			if(strncmp(currentName, name, nameLength) == 0 )
			{
				// We found our entry! 
				printf("Sweet! The names match too!");
			}
			else
			{
				// The names don't match... 
				printf("Error: name does not match."); 
			}

			printf("\nhere"); 

		}

		i++; 

	}

	return 0; 
}

static const int MAXPENDING = 5; 		// Maximum outstanding connection requests 

void HandleTCPClient(int clntSocket); 

int main(int argc, char* argv[])
{
	char ch; 
	int i = 0; 

	initialize_datatable(); 

	printf("Hello, welcome to the SERVER PROGRAM!\n\n"); 

	/*printf("%d arguments provided\nArguments: \n", argc); 

	for(i=0; i<argc; i++)
	{
		printf("\targv[%d] : %s\n", i, argv[i]); 
	}*/ 

	if(argc != 2)
		DieWithUserMessage("Parameters:", "<Server Port>"); 

	if(argv[1] != 0)
		printf("Port # on which to listen : %s\n", argv[1]); 

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
			printf("\nHandling client %s/%d\n", clntName, ntohs(clntAddr.sin_port));


			// Send a welcome message to the client 
			char* welcomeMessage = "Welcome to The Server\n";

			printf("\nAttempting to send a welcome message...");
			send(clntSock, welcomeMessage, strlen(welcomeMessage), 0); 
			printf("\nSuccessfully sent a welcome message.\n"); 

			char id_number[9]; 
			char name[20]; 
			int successfulID = 0; 
			int successfulName = 0; 




			// Receive the 8-digit, newline terminated ID number from the client 
			int expectedLength = 9; 			// Hard-coded, but that's protocol I guess
			unsigned int totalBytesRcvd = 0; 		// Count the total number of bytes received 

			ssize_t numBytes = recv(clntSock, id_number, 9 - 1, 0);

			if(numBytes < 0)
				DieWithSystemMessage("recv() failed\n"); 
			else if(numBytes == 0)
				DieWithUserMessage("recv()", "connection closed prematurely\n"); 
			else if(numBytes != 8)
				DieWithUserMessage("recv()", "failed to receive 8-digit ID number\n"); 

			// If we reach here, we know we've successfully received the ID number
			successfulID = 1; 
			totalBytesRcvd += numBytes; 			// Keep a tally of total bytes

			id_number[numBytes] = '\0'; 

			printf("\nReceived %zu bytes for the 8-digit ID number: %s", numBytes, id_number); 

			printf("\nID number: %s", id_number); 

			// Receive the (up to) 20 character, newline terminated name from the client 
			expectedLength = 20; 			// Have to be prepared for at most 20 characters, it's the protocol!!
			numBytes = recv(clntSock, name, 20 - 1, 0);

			if(numBytes < 0)
				DieWithSystemMessage("recv() failed"); 
			else if(numBytes == 0)
				DieWithUserMessage("recv()", "connection closed prematurely"); 

			// Similar to above, if we make it here we know we've successfully received a name 
			successfulName = 1; 

			totalBytesRcvd += numBytes; 			// Keep a tally of total bytes

			name[numBytes] = '\0'; 

			printf("\nReceived %zu bytes for the (up to) 20 character name: %s", numBytes, name); 
			printf("Name: %s", name); 

			numBytes = 0; 

			int numFailures = 0; 			// Number of failed attemps 

			while((!successfulName && !successfulID) && numFailures < 3)
			{
				printf("\nFailed to receive ID number or name, sending 'Failure' message to client."); 
				// If either one of them has failed, send a failure message back  
				char* failure = "Failure"; 
				numBytes = send(clntSock, failure, strlen(failure), 0); 

				if(numBytes < 0)
					DieWithSystemMessage("send() failed\n"); 
				else if(numBytes != 7)
					DieWithUserMessage("send()", "sent unexpected number of bytes"); 

				printf("Successfully sent (%zu bytes) to the server... Name: %s \n", numBytes, "Failure"); 

				// Try again 
				ssize_t numBytes = recv(clntSock, id_number, 9 - 1, 0);

				if(numBytes < 0)
					DieWithSystemMessage("recv() failed\n"); 
				else if(numBytes == 0)
					DieWithUserMessage("recv()", "connection closed prematurely\n"); 
				else if(numBytes != 8)
					DieWithUserMessage("recv()", "failed to receive 8-digit ID number\n"); 

				// If we reach here, we know we've successfully received the ID number
				successfulID = 1; 
				totalBytesRcvd += numBytes; 			// Keep a tally of total bytes

				id_number[numBytes] = '\0'; 

				printf("\nReceived %zu bytes for the 8-digit ID number: %s", numBytes, id_number); 

				printf("\nID number: %s", id_number); 

				// Receive the (up to) 20 character, newline terminated name from the client 
				expectedLength = 20; 			// Have to be prepared for at most 20 characters, it's the protocol!!
				numBytes = recv(clntSock, name, 20 - 1, 0);

				if(numBytes < 0)
					DieWithSystemMessage("recv() failed"); 
				else if(numBytes == 0)
					DieWithUserMessage("recv()", "connection closed prematurely"); 

				// Similar to above, if we make it here we know we've successfully received a name 
				successfulName = 1; 

				totalBytesRcvd += numBytes; 			// Keep a tally of total bytes

				name[numBytes] = '\0'; 

				printf("\nReceived %zu bytes for the (up to) 20 character name: %s", numBytes, name); 
				printf("Name: %s", name); 

				numBytes = 0; 

				if(!successfulName && !successfulID)
					numFailures++; 
			}

			if(numFailures == 3)			// We've failed 3 times, close the socket 
				close(clntSock); 

			if(!successfulName || !successfulID)
			{
				printf("\nFailed to receive ID number or name, sending 'Failure' message to client."); 
				// If either one of them has failed, send a failure message back  
				char* failure = "Failure"; 
				numBytes = send(clntSock, failure, strlen(failure), 0); 

				if(numBytes < 0)
					DieWithSystemMessage("send() failed\n"); 
				else if(numBytes != 7)
					DieWithUserMessage("send()", "sent unexpected number of bytes"); 

				printf("Successfully sent (%zu bytes) to the server... Name: %s \n", numBytes, "Failure"); 
				
			}
			else
			{
				printf("\nSuccessfully received ID number and name, sending 'Success' message to client"); 
				// We've received the ID number and name successfully, so send a success message back 
				char* success = "Success"; 
				numBytes = send(clntSock, success, strlen(success), 0); 
				
				if(numBytes < 0)
					DieWithSystemMessage("send() failed\n"); 
				else if(numBytes != 7)
					DieWithUserMessage("send()", "sent unexpected number of bytes"); 

				printf("\nSuccessfully sent a Success message to the client... Only cost us %zu bytes! \n", numBytes, "Success");

				// Look up the ID number and username in the datatable array 
				// The format of the data is xxxxxxxx yyyyyyyyyyyyyyyyyyyy
				LookUpIDNumberAndUsername(id_number, name); 


			}

		}		 
		else
			printf("Unable to get client address"); 

		


	}






	printf("Press any key to exit..."); 
	ch = getchar(); 
	return 0; 
}

void HandleTCPClient(int clntSocket)
{
	char buffer [BUFSIZE]; 		// Buffer for string

}