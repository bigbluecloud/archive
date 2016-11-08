/*
*	Assignment 1, server file I/O
*	Jared Rankin, s2896344
*/

#include "../common.h"

//Helper function to determine what flags were passed by the user -Modified version of the function used in listing.c-
void parseInput(int* f, char* fileNames[2], char* input[8])
{
	int i, z = 0;//Ignore index 1 of input[], it will always contain 'put'
	
	for(i = 1; i <= sizeof(input); i++)//Iterate through the user input array
	{
		if(input[i] == NULL)
			break;
	
		if(strcmp(input[i], "-f") == 0 && *f != 1)
			*f = 1;//-f flag was typed
			
		else //Always assumes the local file is given first and the new file name second
		{
			fileNames[z] = input[i];
			z++;
		}
	}
}

void putFileFromClient(int client, char* input[8])
{

	FILE* file;
	int fFlag = 0;//Test if -f is in the command array
	int* f = &fFlag;
	char* fileNames[2] = {'\0'};//Array to hold the file name flags -[0] always holds the local file name, [1] always holds the new file name-
	char string[128] = {'\0'};//Generic reusable string	
	
	parseInput(f, fileNames, input);
	
	send(client, "put", 8, 0);//Tell the client it will be doing it's 'put' request now
	send(client, fileNames[0], sizeof(string), 0);//Send the client the name of the file it said it was going to send -Limited to 128 characters-
	
	recv(client, string, sizeof(string), 0);//Check if the client had an error opening the file
	if(string[0] != '\0')//If there is an error
	{
		printf("%s\n", string);
		return;
	}

	if(fFlag == 1)//A file is being overwritten
	{
		if(fileNames[1] != NULL)
			file = fopen(fileNames[1], "wb");//Use the new file name given by the client
			
		else
			file = fopen(fileNames[0], "wb");//Use the original file name from the client
	}
	
	else //Writing to a new file
	{
		if(fileNames[1] != NULL)
			file = fopen(fileNames[1], "rb");
			
		else
			file = fopen(fileNames[0], "rb");
		
		if(file != NULL)//The file exists
		{
			sprintf(string, "File already exists!\nTry again specifying a new file name or '-f'\n");
			send(client, string, sizeof(string), 0);
			fclose(file);
			return;//Break from the function
		}
		
		if(fileNames[1] != NULL)//The file doesn't exist
			file = fopen(fileNames[1], "wb");
			
		else
			file = fopen(fileNames[0], "wb");
	}
	
	
	if(file == NULL)//The file could not be opened
	{
		sprintf("Error: %s\n", strerror(errno));
		send(client, string, sizeof(string), 0);
		return;
	}
	send(client, string, sizeof(string), 0);//Send a blank packet to show no error happened
	
	//All is well and writing to file can begin!
	do
	{
		recv(client, string, sizeof(string), 0);
		fputs(string, file);		
	}while(string[0] != '\0');
	
	fflush(file);
	fclose(file);
	
	if(fileNames[1] != NULL)
		printf("Copied data into %s\n", fileNames[1]);
	
	else
		printf("Copied data into %s\n", fileNames[0]);
}

void getFileForClient(int client, char* input[8])
{
	FILE* file;
	int i = 0, count = 0, readSize = 64;
	int fFlag = 0, lFlag = 0;
	int* f = &fFlag;
	long byteSize;
	
	char* fileDetail[2] = {'\0'};//Array to hold the file names -[0] always holds the server file, [1] always holds file name to write to-
	char* fileNames[256] = {'\0'};
	
	char str[256] = {'\0'};//Generic string for file paths/names/errors
	char str64[64] = {'\0'};//Secondary generic string used in smaller sendings
	
	parseInput(f, fileNames, input);
	
	send(client, "get", 8, 0);
	
	file = fopen(fileNames[0], "rt");//Open the file on the server
	if(file == NULL)//File could not be opened
	{
		printf("Unable to open file %s\n%s\n", fileNames[0], strerror(errno));
		sprintf(str, "Error: %s\n", strerror(errno));
		send(client, str, sizeof(str), 0);
		return;
	}
	
	if(fFlag == 1 && fileNames[1] == NULL)
	{
		sprintf(str, "Improper use of 'get'\nIf [-f] is specified, a [filename] must also be given\n");
		send(client, str, sizeof(str), 0);//Let the client know an error occurred
		fclose(file);
		return;
	}
	
	send(client, str, sizeof(str), 0);//Send a blank string because no errors occurred
	
	if(fileNames[1] != NULL)//A new file name was specified
	{
		if(fFlag == 1)
			str[0] = '$';//The file is being overwritten
		else
			str[0] = '#';//The file shouldn't exist
		
		strcat(str, fileNames[1]);
	}
		
	send(client, str, sizeof(str), 0);//Send the client the 'mode' string (can be blank)
	
	//Get the size of the file
	fseek(file, 0, SEEK_END);//Go to the end of the file
	byteSize = ftell(file);//Get the number of bytes to the end of the file
	
	sprintf(str64, "%ld", byteSize);//ASCII the byte size so it can be send over the network
	send(client, str64, sizeof(str64), 0);//Tell the client the size of the file in bytes
	
	rewind(file);//Go back to the start of the file
	
	//Send the client the file contents	
	if(fileNames[1] != NULL)//Writing to a file
		readSize = 60;//Has to be 4 characters shorter when writing for some reason
	
	while(!feof(file))
	{
		memset(str64, 0, sizeof(str64));//NULL the str64 array
		if(fread(str64, sizeof(char), readSize, file))//Pull a data block from the file
			send(client, str64, sizeof(str64), 0);//Send the data block
	}
	
	fclose(file);
	
	if(fileNames[1] != NULL)//The file contents are being written to another file
	{
		memset(str64, 0, sizeof(str64));
		send(client, str64, sizeof(str64), 0);//Let the writeLargeBlockToFile function know to exit the recv loop
	}
	
}
