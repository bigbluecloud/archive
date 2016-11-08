#ifndef COMM_H
#define COMM_H

#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>


#ifdef WIN32
	#include <windows.h>
	#include <winsock.h>
	#pragma comment(lib, "ws2_32.lib")

#else
	#include <sys/time.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <sys/utsname.h>
	#include <sys/wait.h>
	
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>

	#include <dirent.h>
	#include <unistd.h>
	#include <signal.h>
	
#endif//Endif for WIN32/UNIX specific libraries


#endif
