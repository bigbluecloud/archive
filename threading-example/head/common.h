/*
	Generic header file for system supplied header file #includes
*/

#ifndef COMM_H
#define COMM_H

//Standard C headers
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#ifdef WIN32//Windows

#else//POSIX

	#define RESET "\e[0m"//Resets the text colour
	#define TRED "\e[31m"//Red

	//Standard UNIX header
	#include <unistd.h>
	
	//Pthreads header
	#include <pthread.h>

	//POSIX IPC headers
	#include <sys/ipc.h>
	#include <sys/shm.h>
	
	//POSIX time
	#include <sys/time.h>
	
	//POSIX multiprocessing 'wait' functions
	#include <sys/wait.h>
	
	//Misc POSIX types
	#include <sys/types.h>
	
	//For use of select in client.c
	#include <sys/select.h>

#endif

#include "generic.h"

#define NEWLINE printf("\n");//Quick macro to insert a newline on the terminal
#define ERR(x, y) {perror(x); exit(y);}//Generic print error & exit macro (bad for dynamic resources)

//Macros to shorten typing out commonly used slots
#define NUMBER *data[0]
#define CLIENTFLAG *flags[0]

#endif //COMM_H
