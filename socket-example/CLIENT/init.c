/*
*	Assignment 1, Client socket set up
*	Jared Rankin, s2896344
*/

#include "../common.h"

int connectServer(char* host)
{
#ifdef WIN32
	SOCKET sock;
	WSADATA ws;
	SOCKADDR_IN sockAddr;
	struct hostent* hp;
	
	if(WSAStartup(0x0101, &ws) != 0)
	{
		printf("Error loading the winsock DLL!\nExiting...");
		WSACleanup();
		system("PAUSE");
		return -1;
	}

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(sock == INVALID_SOCKET)
	{
		closesocket(sock);
		printf("Socket creation failed with error: %u\n", WSAGetLastError());
		WSACleanup();
		system("PAUSE");
		return -1;
	}

	printf("Socket descriptor: %d\n", sock);

	if((hp = gethostbyname(host)) == NULL)
	{
		closesocket(sock);
		printf("Failed to resolve hostname!\n");
		WSACleanup();
		system("PAUSE");
		return -1;
	}

	ZeroMemory(&sockAddr, sizeof(sockAddr));
	
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = *((unsigned long*)hp->h_addr);
	sockAddr.sin_port = htons(80);

	if(connect(sock,(SOCKADDR*)(&sockAddr),sizeof(sockAddr)) !=0)//Doesn't return -1
	{
		closesocket(sock);
		printf("Failed to establish connection with server!\n");
		WSACleanup();
		//system("PAUSE");
		return -1;
	}

#else
	struct sockaddr_in server;
	struct hostent* hp;
	int sock = socket(PF_INET, SOCK_STREAM, 0);//Get a socket descriptor
	
	if(sock == -1)
		return -1;//Return -1 if failed
	
	bzero(&server, sizeof(server));//Zero the struct fields
	hp = gethostbyname(host);//Get the host name from the command line
	
	bcopy(hp->h_addr, &(server.sin_addr.s_addr), hp->h_length);//Copy host name into server struct
	
	//Finish setting up the server struct
	server.sin_family = AF_INET;
	server.sin_port = htons(80);
	
	if(connect(sock, (struct sockaddr*)&server, sizeof(server)) == -1)
		return -1;
#endif

	return sock;
}
