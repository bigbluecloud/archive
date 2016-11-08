/*
	Multithreaded program entry point
	Jared Rankin, s2896344
*/

#include "sem.h"
#include "common.h"
#include "server.h"
#include "client.h"

#define SHMSIZE sizeof(unsigned long)*11 //For data array
#define SYNSIZE sizeof(int)*11 //For sync array

//Delete array segment
void deleteArraySegment(void* mem[], int shmid, int type)
{
	switch(type)//Detach parent from the the data memory segment
	{
		case 0:
			shmdt((unsigned long**)mem);
			break;
		case 1:
			shmdt((int**)mem);
			break;
	}
	shmctl(shmid, IPC_RMID, NULL);//Delete the data memory segment
}

//Create array segment
int createArraySegment(void* mem[], int type, int size)
{
	int i, shmid;
	switch(type)
	{
		case 0://Unsigned long array
			if((shmid = shmget(IPC_PRIVATE, sizeof(unsigned long) * size, 0xffff)) < 0)//Attempt to get a shared memory id
				ERR("data shmget() error", -1)
	
			if((mem[0] = (unsigned long*)shmat(shmid, 0, 0)) < 0)//Get a pointer to a location in memory
				ERR("data shmat() error", -1)
			
			for(i = 0; i < size; i++)
				mem[i] = (unsigned long*)*mem + i;//Pointer arithmetic
			break;
		
		case 1://Int array
			if((shmid = shmget(IPC_PRIVATE, sizeof(int)*size, 0xffff)) < 0)
				ERR("flags shmget() error", -1)
	
			if((mem[0] = (int*)shmat(shmid, 0, 0)) < 0)
				ERR("flags shmat() error", -1)
			
			for(i = 0; i < size; i++)
				mem[i] = (int*)*mem + i;
			break;
	}
	return shmid;
}


int main (int argc, char* argv[])
{
#ifdef WIN32

#else
	
	#ifndef _POSIX_THREAD_PROCESS_SHARED
    #error "This platform does not support process shared mutexes/condition variables!"
    #endif
	
	//Shared memory segment arrays
	int dataID, flagsID, progressID;//Shared memory ID's
	int tNum = 32;//Number of threads to put in the thread pool, 32 by default
	
	int* progress[10];//Array for progress reporting
	int* flags[11];//Synchronisation flags, [0] acts as clientFlag
	unsigned long* data[11];//I/O variables, [0] acts as number
	
	//Not process shared
	Semaphore* threadSem = create_semaphore();//For the threads and client
	
	//Get data/sync array shmids and allocate their segments
	dataID = createArraySegment((void*)data, 0, 11);
	flagsID = createArraySegment((void*)flags, 1, 11);
	progressID = createArraySegment((void*)progress, 1, 10);
	
	//Check if a command line argument was given so a non-default number of threads will be put in the thread pool
	if(argv[1] != NULL)
		tNum = atoi(argv[1]);
	
	switch(fork())
	{
		case -1: //Unable to fork
			ERR("fork() error", -1)
		case 0: //Child (server)
		{
			runServer(tNum, data, flags, progress, threadSem);//Abstraction to another file
			
			//Detach the child from the shared memory when exiting
			shmdt(data);
			shmdt(flags);
			
			exit(0);//Exit child process
			break;//Probably not necessary
		}
		default: //Parent (client)
		{
			runClient(data, flags, progress);//Abstraction of client to another file
			break;
		}
	}
	
	if(waitpid((pid_t)-1, 0, 0) > 0)//Wait for the server to shut down
		printf("\nServer has shut down cleanly\n");
	
	//Clean up dynamic data
	deleteArraySegment((void*)data, dataID, 0);
	deleteArraySegment((void*)flags, flagsID, 1);
	deleteArraySegment((void*)progress, progressID, 1);
	destroy_semaphore(threadSem);
	
#endif
	kbPause();//Wait for the user to confirm termination
	return 0;//Program exited normally
}
