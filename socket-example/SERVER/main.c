/*
*	Assignment 1, server software
*	Jared Rankin, s2896344
*/

#include "../common.h"
#include "extern.h"

#ifndef WIN32
void killZombieProc(int sig)
{
	while(waitpid((pid_t)-1, 0, WNOHANG) > 0)//Allow termination of more than one zombie process per call
		printf("Terminated a zombie process!\n");
}
#endif

int main(int argc, char* argv[])
{
#ifdef WIN32
	int n;
	__int64 startTime, endTime, processTime, freq;

	char eTime[256] = {'\0'};
	char pTime[256] = {'\0'};

	char* commands[8];//Array for individual keywords sent by the client
	char buf[64];//Buffer for all input from the client, should only be valid input strings

	SOCKET sock, client = SOCKET_ERROR;
	sock = setupSocket(1);
	
	freq = QueryPerformanceFrequency((LARGE_INTEGER*)&freq);//Caution, doesn't check if it is supported or not

	if(sock < 0)
	{
		printf("Socket descriptor could not be allocated, exiting...\n");
		WSACleanup();
		system("PAUSE");
		return -1;
	}
	
	printf("Waiting for incoming connections...\n\n");

	while(1)
	{
		while(client == INVALID_SOCKET)
		{
			client = accept(sock, NULL, NULL);
		}
		printf("New connection! Client descriptor is %d\n", client);

		for(;;)//Process client requests
		{
			n = 0;
			fflush(stdout);

			memset(commands, 0, sizeof(commands));
			memset(buf, 0, sizeof(buf));

			recv(client, buf, sizeof(buf), 0);//Receive user input from socket
			
			printf("Message from client %d: %s\n", client, buf);
				
			startTime = getTimeStamps();//Get the time the request processing started

			commands[0] = strtok(buf, " \n");

			while(commands[n++])
				commands[n] = strtok(NULL, " \n");//Tokenise the rest of the client's message and send it to the commands array
			
			if(commands[0] != NULL)
			{
				if (strcmp(commands[0], "close") == 0 || strcmp(commands[0], "quit") == 0)//If the client says it wants to close its connection to the server
				{
					send(client, commands[0], sizeof(commands[0]) + 1, 0);//Send acknowledgement of client disconnecting
					closesocket(client);//Close the client's socket
					printf("Client with descriptor %d has disconnected\n", client);
					client = INVALID_SOCKET;
					break;
				}

				else if(strcmp(commands[0], "sys") == 0)
					getSysInfo(client);//Get the system info

				else if(strcmp(commands[0], "put") == 0)
					putFileFromClient(client, commands);//Copy a client file to the server

				else if(strcmp(commands[0], "get") == 0)
					getFileForClient(client, commands);//Copy a server file to the client

				else
					send(client, "Unknown command!\n", 18, 0);//If an invalid command is sent by the client
					
				endTime = getTimeStamps();//Get the time the requst processing ended
				processTime = endTime - startTime;//Time taken to do the processing
				
				processTime *= 1000000000;//For nanoseconds
				processTime /= freq;//Divide by ticks per second

				endTime *= 1000000000;
				endTime /= freq;
			
				//Need to sprintf to a character array
				sprintf(eTime, "%lu", endTime);
				sprintf(pTime, "%lu", processTime);

				//Send the client necessary information to calculate the response time
				send(client, pTime, sizeof(__int64), 0);
				send(client, eTime, sizeof(__int64), 0);
			}

			else
				send(client, "Please enter a command\n", 32, 0);
		}
	}

	closesocket(sock);
	WSACleanup();

#else
	int backlog = 1, pid;
	int sock = setupSocket(backlog);
	
	if(sock < 0)
	{
		printf("Socket descriptor could not be allocated, exiting...\n");
		return -1;
	}
	
	printf("Waiting for incoming connections...\n\n");
	
	while(1)//No way to gracefully exit
	{
		int n, pid, clientCount = 0;
		int clientSock = accept(sock, 0, 0);//Accept a client connection -Blocks until a connection is made-
		
		if(clientSock < 0)
		{
			//Close the socket and print an error on failure
			printf("Unable to accept connection: %s", strerror(errno));
			close(sock);
			return -1;
		}
		
		signal(SIGCHLD, killZombieProc);//Check for children exiting
		printf("New connection! Client descriptor is %d\n", clientSock);
		
		if((pid = fork()) < 0)//Create a child process
		{
			printf("Fork error!");
		}
		
		if(pid == 0) //This is the child process
		{
			printf("New process! PID: %d\n", getpid());
			
			for(;;)//Process client requests
			{
				n = 0;
				fflush(stdout);

				char* commands[8] = {'\0'};//Array for individual keywords sent by the client
				char buf[64] = {'\0'};//Buffer for all input from the client, should only be valid input strings

				recv(clientSock, buf, sizeof(buf), 0);//Receive user input from socket -SEGFAULTS IF BLANK-

				printf("Message from client %d: %s\n", clientSock, buf);
				
				uint32_t startTime = htonl(getTimeStamps());//Get the time the request processing started

				commands[0] = strtok(buf, " \n");

				while(commands[n++])
					commands[n] = strtok(NULL, " \n");//Tokenise the rest of the client's message and send it to the commands array

				if (strcmp(commands[0], "close") == 0 || strcmp(commands[0], "quit") == 0)//If the client says it wants to close its connection to the server
				{
					send(clientSock, commands[0], sizeof(commands[0]) + 1, 0);//Send acknowledgement of client disconnecting
					close(clientSock);//Close the client's socket
					printf("Client with descriptor %d has disconnected\n", clientSock);
					exit(0);//Exit the child process
				}

				else if(strcmp(commands[0], "list") == 0)
					listFiles(clientSock, commands);//Lists the files in the directory -Lists files in any given directory but cannot have any of the other flags applied to the command

				else if(strcmp(commands[0], "delay") == 0)
					delayedInteger(n - 1, commands, clientSock);//Returns the given integer after the specified amount of seconds

				else if(strcmp(commands[0], "sys") == 0)
					getSysInfo(clientSock);//Get the system info

				else if(strcmp(commands[0], "put") == 0)
					putFileFromClient(clientSock, commands);//Copy a client file to the server

				else if(strcmp(commands[0], "get") == 0)
					getFileForClient(clientSock, commands);//Copy a server file to the client

				else
					send(clientSock, "Unknown command!\n", 18, 0);//If an invalid command is sent by the client
					
				uint32_t endTime = htonl(getTimeStamps());//Get the time the requst processing ended
				uint32_t processTime = endTime - startTime;//Time taken to do the processing
				
				//Send the client necessary information to calculate the response time
				send(clientSock, &processTime, sizeof(uint32_t), 0);
				send(clientSock, &endTime, sizeof(uint32_t), 0);
			}
		}
	}
#endif
	return 0;
}
