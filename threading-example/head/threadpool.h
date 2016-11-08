/*
	Thread pool header file
	This implementation is based on the code found at https://github.com/mbrossard/threadpool/blob/master/src/threadpool.c
	Credit to user 'mrbrossard' of github.com
*/
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "sem.h"
#include "common.h"
#include "cygbarrier.h"

typedef struct
{
	unsigned long number;//Number to be factorised
} Task;


//Thread pool struct
typedef struct
{
	Semaphore* waitSem;		//Semaphore for waiting at in the pool
	Semaphore* writeSem;	//Semaphore used when writing to the data array
	pthread_t* threads;		//Array of worker thread ID's
	Task* queue;			//Task queue
	int** progressArr;		//Reference to the progress array
	unsigned long** dMem;	//Shallow copy of the pointer to the data[] in shared memory
	int** fMem;				//Shallow copy of the pointer to the flags[] in shared memory
	barrier_t jobBarrier;	//Barrier used to ensure all threads pick up the job
	int threadCount;		//Number of threads launched
	int queueSize;			//Size of task queue
	int waitingTasks;		//Number of pending tasks
	int head;				//First element in the queue
	int tail;				//Next element in the queue
	int threadNumber;		//Number of the thread, used for rotations
	
} threadpool;

//Function prototypes
threadpool* threadpool_create(int, int, unsigned long**, int**, int**, Semaphore*);
int threadpool_queuejob(threadpool*, unsigned long);
int threadpool_destroy(threadpool*);
void* threadpool_relax(void*);
int threadpool_factorise(unsigned long, int, int, threadpool*);

#endif //THREADPOOL_H
