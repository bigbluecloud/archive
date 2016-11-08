/*
*	Assignment 1, Client software
*	Jared Rankin, s2896344
*/

#include "../common.h"
#include "extern.h"

#define WAIT_TIME 1

#ifndef WIN32
void sigHandle(int sig)
{
	errno = EINTR;
}
#endif

int main(int argc, char* argv[])
{
	int c;

#ifdef WIN32
	SOCKET sock;
	__int64 serverProcTime, serverEndTime, clientRecTime, effectiveResTime;
	char pTime[256] = {'\0'};
	char eTime[256] = {'\0'};

	char rbuf[64] = {'\0'};
	char wbuf[64] = {'\0'};
	
	if(argv[1] == NULL)
		argv[1] = "localhost";//Default to localhost if no server address is specified

	sock = connectServer(argv[1]);
	printf("Connecting to server...\n");
	
	if(sock == INVALID_SOCKET)
	{
		printf("Unable to connect to server \"%s\"\nPress ENTER to exit...", argv[1]);
		do
		{
			c = fgetc(stdin);
		}while((c != '\n') && (c != EOF));
		return -1;
	}
	
	printf("Connection forged!\n\n");

	while(1)
	{
		int n = 0;
		//User types input
		memset(wbuf, 0, sizeof(wbuf));
		printf(">");
		fflush(stdout);
		
		fgets(wbuf, sizeof(wbuf), stdin);//fgets the user input -BLOCKS HERE-
		send(sock, wbuf, sizeof(wbuf), 0);//Send all user input to server
		
		//Read from the socket
		memset(rbuf, 0, sizeof(rbuf));
		
		recv(sock, rbuf, sizeof(rbuf), 0);
			
		//Check cases here
		if(strcmp(rbuf, "quit") == 0 || strcmp(rbuf, "close") == 0)//Allow the user to exit the client software
		{
			printf("Server has acknowledged disconnection request\nClosing socket\n");
			closesocket(sock);
			printf("Press ENTER to exit...");
			
			do
			{
				c = fgetc(stdin);
				exit(0);
			}while((c != '\n') && (c != EOF));
		}
		
		if(strcmp(rbuf, "sys") == 0)
			displaySystem(sock);
		
		else if(strcmp(rbuf, "list") == 0)
			displayList(sock, 0);
			
		else if(strcmp(rbuf, "get") == 0)
			displayList(sock, 1);
		
		else if(strcmp(rbuf, "put") == 0)
			sendFile(sock);
		
		else
			printf("Server replied with message:\n%s\n", rbuf);

		
		clientRecTime = getTimeStamp();
		
		//Receive processing and end time from the server
		recv(sock, pTime, sizeof(pTime), 0);
		recv(sock, eTime, sizeof(eTime), 0);
		
		serverProcTime = atol(pTime);
		serverEndTime = atol(eTime);

		effectiveResTime = serverProcTime + (clientRecTime - serverEndTime);
		
		printf("\nResponse time: %lu nanoseconds\n", effectiveResTime);
		fflush(stdout);
	}
#else
	if(argv[1] == NULL)
		argv[1] = "localhost";//Default to localhost if no server address is specified	

	int sockD = connectServer(argv[1]);
	printf("Connecting to server...\n");
	
	if(sockD < 0)
	{
		printf("Unable to connect to server \"%s\"\nPress ENTER to exit...", argv[1]);
		do
		{
			c = fgetc(stdin);
		}while((c != '\n') && (c != EOF));
		return -1;
	}
	
	printf("Connection forged!\n\n");

	while(1)
	{
		int n = 0;
		//User types input
		char wbuf[64] = {'\0'};//Write buffer
		printf(">");
		fflush(stdout);
		
		fgets(wbuf, sizeof(wbuf), stdin);//fgets the user input -BLOCKS HERE-
		send(sockD, wbuf, sizeof(wbuf), 0);//Send all user input to server
		
		//Read from the socket
		char rbuf[64] = {'\0'};//Read buffer
		
		//-NON BLOCKING DOESN'T ACTUALLY WORK, TESTED WITH 'delay' COMMAND-
		signal(SIGALRM, sigHandle);//Affects response times badly because the above code will block for the user to type
		alarm(WAIT_TIME);//Wait one second
		
		if(recv(sockD, rbuf, sizeof(rbuf), 0) < 0)//No data is received
		{
			if(errno == EINTR)//Timed out
				continue;//Restart the loop
			else
				printf("%s", strerror(errno));
		}
		else
			alarm(0);//Turn off the alarm
			
		//Check cases here
		if(strcmp(rbuf, "quit") == 0 || strcmp(rbuf, "close") == 0)//Allow the user to exit the client software
		{
			printf("Server has acknowledged disconnection request\nClosing socket\n");
			close(sockD);
			printf("Press ENTER to exit...");
			
			do
			{
				c = fgetc(stdin);
				exit(0);
			}while((c != '\n') && (c != EOF));
		}
		
		if(strcmp(rbuf, "sys") == 0)
			displaySystem(sockD);
		
		else if(strcmp(rbuf, "list") == 0)
			displayList(sockD, 0);
			
		else if(strcmp(rbuf, "get") == 0)//Get will ruin everything...
			displayList(sockD, 1);
		
		else if(strcmp(rbuf, "put") == 0)
			sendFile(sockD);
		
		else
			printf("Server replied with message:\n%s\n", rbuf);

		uint32_t serverProcTime, serverEndTime;
		unsigned long clientRecTime = getTimeStamp();
		
		//Receive processing and end time from the server
		recv(sockD, &serverProcTime, sizeof(uint32_t), 0);
		recv(sockD, &serverEndTime, sizeof(uint32_t), 0);
		
		unsigned long effectiveResTime = ntohl(serverProcTime) + (clientRecTime - ntohl(serverEndTime));
		
		printf("\nResponse time: %d nanoseconds\n", effectiveResTime);
		fflush(stdout);
	}
#endif
}
