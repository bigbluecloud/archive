/*
*	Assignment 1, Client file I/O
*	Jared Rankin, s2896344
*/

#include "../common.h"

void writeToFile(char formattedStr[4096], char filePath[256], char mode)
{
	FILE* file;
	
	if(mode == '!')//Overwriting a file
		file = fopen(filePath, "wt");//Open in write binary mode (creates if it doesn't exist)
	
	else //Writing to a new file
	{
		file = fopen(filePath, "rb");
		if(file != NULL)//The file exists
		{
			printf("File already exists!\nTry again with a new file name or specify '-f'\n");
			fclose(file);
			return;//Break from the function
		}
		
		else //File does not exist
			file = fopen(filePath, "wt");//Create and open the new file
	}
	
	fputs(formattedStr, file);//Write the formatted list to file
	fflush(file);//Flush the memory buffers
	fclose(file);//Close the file
	
	if(mode == '!')//Give the user feedback
		printf("File %s was forcibly overwritten\n", filePath);
		
	else
		printf("Directory listing was printed in %s\n", filePath);
	
}

void writeLargeBlockToFile(int sock, char path[256], char mode)
{
	FILE* file;
	char* buffer;
	char string[64] = {'\0'};
	char fileSizeStr[256] = {'\0'};
	long fileSize;
	
	recv(sock, fileSizeStr, sizeof(fileSizeStr), 0);//Get the file size
	fileSize = atol(fileSizeStr);//Convert the ASCII to a long
	
	buffer = (char*)malloc(sizeof(char)*fileSize);//Allocate buffer size on the heap
	
	do //Receive all of the data from the server
	{
		recv(sock, string, sizeof(string), 0);
		strcat(buffer, string);//Add the newest string on to the end of the buffer
	}while(string[0] != '\0');
	
	if(mode == '$')//Overwriting a file
		file = fopen(path, "wt");//Open in write binary mode (creates if it doesn't exist, truncates if it does)
	
	else //Writing to a new file
	{
		file = fopen(path, "rb");
		if(file != NULL)//The file exists
		{
			printf("File already exists!\nTry again with a new file name or specify '-f'\n");
			fclose(file);
			free(buffer);//Have to plug those leaks!
			return;//Break from the function
		}
		
		else //File does not exist
			file = fopen(path, "wt");//Create and open the new file
	}
	
	fwrite(buffer, sizeof(char), fileSize, file);//Writes the buffer to file
	fflush(file);//Flush the data to disk
	fclose(file);//Close the file
	
	free(buffer);//Deallocate the buffer
	
	if(mode == '$')//Give the user feedback
		printf("File %s was forcibly overwritten\n", path);
		
	else
		printf("File %s was copied successfully\n", path);
}

void sendFile(int sock)
{
	FILE* file;
	char str[128] = {'\0'};//Generic reusable string

	recv(sock, str, sizeof(str), 0);//Receive the name of the file to send
	
	//Attempt to open the file
	file = fopen(str, "rb");
	
	if(file == NULL)//The file could not be opened
	{
		printf("Unable to open file %s\n%s\n", str, strerror(errno));//Give the user feedback
		sprintf(str, "Client error!\n%s\n", strerror(errno));//Reuse str to send the server an error
		send(sock, str, sizeof(str), 0);
		return;
	}
	
	//File opened successfully, NULL str and send that to server to indicate no error
	sprintf(str, "\0");//Re-NULL the string
	send(sock, str, sizeof(str), 0);
	
	
	//Check if an error occurred on the server
	recv(sock, str, sizeof(str), 0);
	if(str[0] != '\0')//An error occurred
	{
		printf("%s\n", str);
		fclose(file);
		return;
	}
	
	while(!feof(file))
	{
		if(fgets(str, sizeof(str), file))//Pull a string from the file
			send(sock, str, sizeof(str), 0);//Send it to the server
	}
	
	sprintf(str, "\0");
	send(sock, str, sizeof(str), 0);//Tell the server all of the file has been sent
	
	fflush(file);//For peace of mind
	fclose(file);
	
	printf("Sent file to server\n");
}
