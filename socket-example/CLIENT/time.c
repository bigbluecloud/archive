/*
*	Assignment 1, Client time functions
*	Jared Rankin, s2896344
*/

#include "../common.h"

unsigned long getTimeStamp()
{
#ifdef WIN32
	__int64 freq, time;

	freq = QueryPerformanceFrequency((LARGE_INTEGER*)&freq);//Caution, doesn't check if it is supported or not
	QueryPerformanceCounter((LARGE_INTEGER*)&time);
	
	//Get nanoseconds
	time *= 1000000000;
	time /= freq;
	
	return time;

#else
	struct timespec ts;
	
	clock_gettime(CLOCK_REALTIME, &ts);//Store the current time into the ts struct
	return ts.tv_nsec;
#endif
}
