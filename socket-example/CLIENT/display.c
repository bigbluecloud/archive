/*
*	Assignment 1, Client stdout output
*	Jared Rankin, s2896344
*/

#include "../common.h"

void displayList(int sock, int type)
{
	long i = 0, cc;
	char genString[256] = {'\0'};//Used for displaying errors and checking if the list is being written to file
	
	recv(sock, genString, sizeof(genString), 0);//Check if there was an error on the server
	if(genString[0] != (int)NULL)//If the first character of the string is NULL it can be assumed there is no error
	{
		printf("%s\n", genString);
		return;
	}
	
	recv(sock, genString, sizeof(genString), 0);//Receive the write mode and file path (if any)
	if(genString[0] != (int)NULL)//A file path and writing mode has been passed.
	{
		char mode = genString[0];
		char strBlock[4096] = {'\0'};
		
		for(i = 0; i < sizeof(genString) - 1; i++)//Slow algorithm
		{
			if(genString[i] == (int)NULL)
				break;
			genString[i] = genString[i + 1];//Get the file path (removing the mode character)
		}
		
		if(mode == '!' || mode == '@')
		{
			recv(sock, strBlock, sizeof(strBlock), 0);//Receive the single block of data to write
			writeToFile(strBlock, genString, mode);//Write the listing to file
			return;//Exit the function here
		}
		
		else
		{
			writeLargeBlockToFile(sock, genString, mode);
			return;
		}
	}
	
	recv(sock, genString, sizeof(genString), 0);//Get the number of file entries/lines the server will be sending (ASCII form)
	cc = atol(genString);//ASCII to long the genString array to get the number of file entries
	
	if(type == 1)//If this is a 'get', not a 'list'
		cc = (cc / 64) + 1;//Account for the size of incoming blocks from 'get' command
	
	for(i = 0; i < cc; i++)//Print contents to stdout
	{
		if(type == 1)
			recv(sock, genString, sizeof(genString) / 4, 0);//Only read 64 bytes at a time
		else
			recv(sock, genString, sizeof(genString), 0);
		
		if(i != 0 && i % 40 == 0)//If i is a multiple of 40 pause the listing
		{
			int c;
			printf("\nPress ENTER to continue...\n");
			do
			{
				c = fgetc(stdin);
			}while((c != '\n') && (c != EOF));
		}
		if(type == 0)
			printf("%s\n", genString);
		else
			printf("%s", genString);
	}
	
	printf("\n\n");
}
