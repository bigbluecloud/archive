/*
*	Assignment 1, Server externs
*	Jared Rankin, s2896344
*/

#ifndef EXTERNS_H
#define EXTERNS_H

	
	extern void listFiles(int, char*[]);
	extern void putFileFromClient(int, char*[]);
	extern void getFileForClient(int, char*[]);
	
	extern long getTimeStamps();
	extern void delayedInteger(int, char*[], int);
	
	extern void getSysInfo(int);
	
	extern int setupSocket(int);

#endif