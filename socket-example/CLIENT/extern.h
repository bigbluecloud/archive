/*
*	Assignment 1, Client Externs
*	Jared Rankin, s2896344
*/

#ifndef EXTERNH
#define EXTERNH

extern int connectServer(char*);

extern void displaySystem(int);
extern void displayList(int, int);

extern void writeToFile(char[], char[], char);
extern void writeLargeBlockToFile(int, char[], char);
extern void sendFile(int);

extern unsigned long getTimeStamp();

#endif
