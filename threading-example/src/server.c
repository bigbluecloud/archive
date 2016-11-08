/*
	Server process main file
*/

#include "server.h"

//Basis for the server part of the program
void runServer(int threadNum, unsigned long* data[], int* flags[], int** progArr, Semaphore* poolSem)
{
	int ret, i;
	threadpool* pool = threadpool_create(threadNum, 10, data, flags, progArr, poolSem);//Create a threadpool with the passed number of threads and a queue size of 10
	
	//Start job handling loop
	while(1)
	{		
		while(CLIENTFLAG == 0)//Busy waiting because Cygwin can't initialise a process shared mutex
			usleep(5000);//5ms sleep to reduce busy waiting
		
		if(CLIENTFLAG == -1)
		{
			threadpool_destroy(pool);//Destroy the thread pool
			break;//Exit the loop
		}
		
		if((ret = threadpool_queuejob(pool, NUMBER)) != 0)//Read the data from number and add it as a job to threadpool queue
		{
			if(ret == -1)
				printf("There isn't a valid thread pool to add the job to\n");
			if(ret == -2)
				printf("Pool semaphore lock/unlock failure\n");
			if(ret == -3)
				printf("The queue is full, try again later\n");
		}
		
		//printf("Number %lu added to queue\n", NUMBER);//Mostly used for troubleshooting, it doesn't flush to the terminal straight away
		//fflush(stdout);
		
		CLIENTFLAG = 0;//Set client flag back to 0 because the job has been read
		
		if(broadcast_sem(pool->waitSem) != 0)//Let all threads know there is a job
			printf("Broadcast to pool semaphore failed\n");
	}
}
