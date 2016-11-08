/*
*	Assignment 1, server socket set up
*	Jared Rankin, s2896344
*/

#include "../common.h"

int setupSocket(int backlog)
{
#ifdef WIN32
	SOCKET sock;
	WSADATA ws;
	SOCKADDR_IN sockAddr;
	
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
	ZeroMemory(&sockAddr, sizeof(sockAddr));
	
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockAddr.sin_port = htons(80);

	if(bind(sock, (SOCKADDR*)(&sockAddr), sizeof(sockAddr)) == SOCKET_ERROR)
	{
		closesocket(sock);
		printf("Bind failed with error: %u\n", WSAGetLastError());
		WSACleanup();
		system("PAUSE");
		return -1;
	}
	
	printf("Socket bound!\n");

	listen(sock, backlog);
	
	printf("Listening on port %d\n", ntohs(sockAddr.sin_port));
	return sock;
	
#else
	struct sockaddr_in sAddr;//socket_in struct
	
	int sock_id = socket(PF_INET, SOCK_STREAM, 0);
	if(sock_id == -1)
	{
		//Close the socket and print an error on failure
		close(sock_id);
		printf("Unable to create socket: %s\n", strerror(errno));
		return -1;
	}
	
	printf("Socket descriptor: %d\n", sock_id);
	
	bzero((void*)&sAddr, sizeof(sAddr));//Set all fields to NULL for the sAddr struct
	
	sAddr.sin_addr.s_addr = htonl(INADDR_ANY);//Allow any network interface to be used
	sAddr.sin_port = htons(80);//Have the port converted to network byte order (short) and assigned to the port field in sAddr
	sAddr.sin_family = AF_INET;//Assign the protocol family
	
	if(bind(sock_id, (struct sockaddr*)&sAddr, sizeof(sAddr)) != 0)//Ensure the socket is bound properly
	{
		//Close the socket and print an error on failure
		close(sock_id);
		printf("Unable to bind to socket:\n%s\n", strerror(errno));
		return -1;
	}
	
	printf("Socket bound!\n");
	
	if(listen(sock_id, backlog) != 0)//Wait for incoming connections
	{
		//Close the socket and print an error on failure
		close(sock_id);
		printf("Unable to listen on socket: %s", strerror(errno));
		return -1;
	}
	
	printf("Listening on port %d\n", ntohs(sAddr.sin_port));
	
	return sock_id;
#endif
}
