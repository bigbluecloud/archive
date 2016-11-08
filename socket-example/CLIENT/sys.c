/*
*	Assignment 1, Client system display
*	Jared Rankin, s2896344
*/

#include "../common.h"

void displaySystem(int sock)
{
	char serverInfo[2048] = {'\0'};
	recv(sock, serverInfo, sizeof(serverInfo), 0);
	
	printf("Server system information:\n%s\n", serverInfo);	
}
