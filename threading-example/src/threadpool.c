/*
	Thread pool implementation
*/

#include "threadpool.h"

//Allocate the thread pool
threadpool* threadpool_create(int threadNum, int queueSz, unsigned long** dPtr, int** fPtr, int** progress, Semaphore* sem)
{
	threadpool* pool;
	int i;
	
	if((pool = (threadpool*)malloc(sizeof(threadpool))) == NULL)
		return NULL;//Thread pool could not be allocated
	
	//Initialise the thread pool members
	pool->progressArr = progress;
	pool->dMem = dPtr;
	pool->fMem = fPtr;
	pool->threadCount = 0;
	pool->queueSize = queueSz;
	pool->waitingTasks = 0;
	pool->head = 0;
	pool->tail = 0;
	pool->threadNumber = 0;
	
	//Allocate the thread ID array and queue
	pool->threads = (pthread_t*)malloc(sizeof(pthread_t) * threadNum);
	pool->queue = (Task*)malloc(sizeof(Task) * queueSz);
	
	//Create the semaphores
	pool->waitSem = create_semaphore();
	pool->writeSem = sem;
	
	//Create the barrier
	if(barrier_init(&pool->jobBarrier, threadNum) != 0)//Initialise the barrier
		printf("barrier_init error\n");//Print an error on fail
	
	if(pool->waitSem == NULL || pool->threads == NULL || pool->queue == NULL || pool->writeSem == NULL)//Ensure everything was allocated
	{
		threadpool_destroy(pool);
		return NULL;
	}
	
	//Launch the threads
	for(i = 0; i < threadNum; i++)
	{
		pool->threadNumber = i;//Reassign the thread number
	
		if(pthread_create(&(pool->threads[i]), NULL, threadpool_relax, (void*)pool) != 0)
		{
			threadpool_destroy(pool);
			return NULL;
		}
		pool->threadCount++;//Increment active thread counter
		usleep(1000);//Stop the loop running too fast
	}
	return pool;
}

int threadpool_destroy(threadpool* pool)
{	
	if(pool == NULL)
		return -1;//invalid pool
	
	if(pool->waitSem)//If the pool allocated the wait semaphore
		destroy_semaphore(pool->waitSem);
	
	if(pool->queue)//If the queue was allocated
		free(pool->queue);
	
	if(pool->threads)//If the thread ID array was allocated
		free(pool->threads);
	
	barrier_destroy(&pool->jobBarrier);
	
	free(pool);//Destroy the thread pool
	return 0;//Function succeeded
}

int threadpool_queuejob(threadpool* pool, unsigned long argument)
{
	int next;//For adding jobs to the queue

	if(pool == NULL)
		return -1;//There isn't a valid threadpool
	
	if(lock_sem(pool->waitSem) != 0)
		return -2;//Lock/unlock failure
	
	next = pool->tail + 1;//Go to the next index in the array
	next = (next == pool->queueSize) ? 0 : next;//Loop back to the start of the queue if next is equal to queueSize (i.e. tail is at the last index of the queue array, 9)
	
	if(pool->waitingTasks == pool->queueSize)//Is the queue full?
		return -3;//Queue full
	
	pool->queue[pool->tail].number = argument;//Add the new number that will be factorised into the queue
	pool->tail = next;//Move the current job index forward
	pool->waitingTasks += 1;//Increment the waiting tasks counter
	
	if(unlock_sem(pool->waitSem) != 0)
		return -2;//Lock/unlock failure
		
	return 0;//Function succeeded
}

void* threadpool_relax(void* tp)
{
	threadpool* pool = (threadpool*)tp;
	Task job;
	int tNumber = pool->threadNumber;//Create a local copy of the thread number so it doesn't read the changing value from the struct
	int queryNumber;//Declaration of query number which will be grabbed by the threads
	
	while(1)//Infinite loop
	{
		//Lock the mutex, so the condition variable can be waited on
		if(lock_sem(pool->waitSem) != 0)//Should block here
			printf("Error locking pool semaphore\n");
		
		while(pool->waitingTasks == 0)//While there are no jobs on the queue
			wait_sem(pool->waitSem);//Wait for a job to come in
		
		//Get the job
		job.number = pool->queue[pool->head].number;//Get the number to factorise
		queryNumber = pool->head + 1;//Get the query number, to be used writing to the right server slot. + 1 because head can be 0 to 9 but the values are given to data memory slots 1 to 10
		
		if(unlock_sem(pool->waitSem) != 0)//Unlock the mutex and get to work
			ERR("thread unlock_sem error", -1)//thread lock/unlock error
		
		
		if(*pool->progressArr[9] != 0 && tNumber == 0)//If there have been 10 queries done previously
			{
				int i;
				for(i = 0; i < 10; i++)
					*pool->progressArr[i] = 0;//Reset all of the progress array values to zero
			}
		
		barrier_wait(&pool->jobBarrier);//Wait for all threads to grab the job
		
		if(tNumber == 0)//Have the 0th thread remove the job from the queue
		{
			//Move the job off the queue
			pool->head += 1;
			pool->head = (pool->head == pool->queueSize) ? 0 : pool->head;//Loop back to the start of the queue if the head counter is bigger than the queue size
			pool->waitingTasks--;//Decrement the number of waiting tasks
		}
		
		threadpool_factorise(job.number, tNumber, queryNumber, pool);//Factorise the number
	}
}

int threadpool_factorise(unsigned long num, int rotations, int queryNumber, threadpool* pool)
{
	struct timespec ts;
	int i;
	//Get the rotated number
	unsigned long c = num << (32 - rotations);
	unsigned long rotNum = (num >> rotations) | c;
	
	long startTime;
	
	clock_gettime(CLOCK_REALTIME, &ts);//Get the time the division started
	startTime = ts.tv_nsec;
	
	//Trial division method
	for(i = 2; i < rotNum - 1; i++)//For every number from 2 to the rotated number - 1
	{
		if(rotNum % i == 0)//Test if rotNum modded by i gives a whole number
		{
			lock_sem(pool->writeSem);//Enter critical section
			
			*pool->fMem[queryNumber] = 1;//Set a flag so the client knows there is a number to return
			*pool->dMem[queryNumber] = (rotNum / i);//Put the factor in the server slot
			
			while(*pool->fMem[queryNumber] != 0)//Block the thread from unlocking while the client hasn't read the number -Busy waiting-
				usleep(5000);
			
			unlock_sem(pool->writeSem);//Exit critical section
		}
	}
	
	*pool->progressArr[queryNumber - 1] += (100/pool->threadCount);//Not entirely accurate as it is an int, rounds down. - 1 because the query number is 'aligned' to the data slots
	/*printf(TRED " Thread %d finished " RESET, rotations);//Left this here so it can be seen when threads finish
	fflush(stdout);*/
	barrier_wait(&pool->jobBarrier);//Join barrier again to wait for all threads to finish factorisation
	
	if(rotations == 0)//Have the 0th thread print out that all threads are finished
	{
		*pool->progressArr[queryNumber - 1] = 100;//Set the status of the query as 100% complete
		clock_gettime(CLOCK_REALTIME, &ts);
		*pool->dMem[queryNumber] = ts.tv_nsec - startTime;
		*pool->fMem[queryNumber] = -1;
	}
	return 0;
}
