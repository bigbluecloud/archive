/*
*	Assignment 1, server clock functions
*	Jared Rankin, s2896344
*/

#include "../common.h"

void sleepFunc(int timeDelay)
{
#ifdef WIN32

#else
	long delay;
	clock_t now, then;
	
	delay = timeDelay * CLOCKS_PER_SEC;//Calculate the delay interval in cycles per second -CLOCKS_PER_SEC is a macro defined in time.h-
		
	now = then = clock();//Assign clock values to now and then;
		
	while((now - then) < delay)//Loop until then - start is equal to or greater than the delay interval (input time the number of clock cycles per second)
		now = clock();//Assign a new value to now
#endif
}


void delayedInteger(int givenArgs, char* input[8], int cSock)
{
	char errStr[64] = "Improper use of 'delay'\nUse only a single digit\n";
	
	if(givenArgs != 2)//Ensure that the correct number of given arguments were given
	{
		send(cSock, errStr, sizeof(errStr), 0);//Send an error message to the client
		return;
	}
#ifdef WIN32

#else	
	char message[64] = {'\0'};//For a formatted message
	strcat(message, input[1]);
	strcat(message, "\n");
	
	sleepFunc(atoi(input[1]));
		
	send(cSock, message, sizeof(message), 0);//Send the integer back to the client
#endif
}

unsigned long getTimeStamps()
{

#ifdef WIN32
	__int64 time;

	time = QueryPerformanceCounter((LARGE_INTEGER*)&time);

	return time;

#else
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	
	return ts.tv_nsec;
#endif
}
