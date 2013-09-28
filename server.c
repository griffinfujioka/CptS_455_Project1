#include <stdio.h>

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

	if(argv[1] != 0)
		printf("Port # on which to listen : %s\n", argv[1]); 





	printf("Press any key to exit..."); 
	ch = getchar(); 
	return 0; 
}