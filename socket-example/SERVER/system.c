/*
*	Assignment 1, server system information
*	Jared Rankin, s2896344
*/

#include "../common.h"

//Doesn't quite work for WIN32, osVer gets corrupted on the stack?
//Still returns data though
void getSysInfo(int client)
{
	char system[2048];
	char* errStr = "Error retrieving system information\n\n";

	#ifdef WIN32
	OSVERSIONINFO osVer;
	SYSTEM_INFO sysInf;
	DWORD size = 16;//So windows is happy
	char computerName[16] = {'\0'};

	ZeroMemory(&osVer, sizeof(OSVERSIONINFOEX));
	ZeroMemory(&sysInf, sizeof(SYSTEM_INFO));

	osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	send(client, "sys", 8, 0);//Tell the client it is going to receive data from a 'sys' request

	if(!(GetVersionEx((OSVERSIONINFO*) &osVer)))
		send(client, errStr, sizeof(errStr), 0);

	else
	{
		GetSystemInfo(&sysInf);//No error checking!
		GetComputerName(computerName, &size);

		sprintf(system, "COMPUTER NAME: %s\nOS MAJOR VERSION.MINOR VERSION: %lu.%lu\nBUILD NUMBER: %lu\nPROCESSOR TYPE: %lu\n", computerName, osVer.dwMajorVersion, osVer.dwMinorVersion, osVer.dwBuildNumber, sysInf.dwProcessorType);
		send(client, system, sizeof(system), 0);
	}


	#else
	struct utsname utsn;
	
	send(client, "sys", 8, 0);//Tell the client it needs to process a 'sys' request it made
	
	if(uname(&utsn) == 0)//System info is retrieved without error
	{
		sprintf(system, "NODE NAME: %s\nOS RELEASE VERSION: %s %s %s\nCPU TYPE: %s\n", utsn.nodename, utsn.sysname, utsn.release, utsn.version, utsn.machine);
		send(client, system, sizeof(system), 0);
	}
	
	else
		send(client, errStr, sizeof(errStr), 0);
	#endif
}
