/*
	Semaphore implementation
*/
#include "sem.h"

//Used to build the semaphore
Semaphore* create_semaphore()
{
	Semaphore* semaphore = (Semaphore*)malloc(sizeof(Semaphore));
	if(semaphore == NULL)//The malloc failed
		return NULL;
	
	semaphore->mutexattr = init_mutexattr();
	semaphore->condattr = init_condattr();
	
	semaphore->mutex = init_mutex(semaphore->mutexattr);
	semaphore->cond = init_cond(semaphore->condattr);
	
	return semaphore;
}

//Deallocates dynamic memory used by the semaphore -Assumes that everything was allocated without error and can deallocate everything-
int destroy_semaphore(Semaphore* sem)
{
	//Destroy condition variable & mutex
	pthread_cond_destroy(sem->cond);
	pthread_mutex_destroy(sem->mutex);
	
	//Destroy the attribute structures
	pthread_condattr_destroy(sem->condattr);
	pthread_mutexattr_destroy(sem->mutexattr);
	
	//Deallocate all of the heap memory used by the semaphore
	free(sem->cond);
	free(sem->mutex);
	free(sem->condattr);
	free(sem->mutexattr);
	free(sem);
	
	return 0;//Function succeeded
}

//Initialise a mutex attribute structure
MutexAttr* init_mutexattr()
{
	int ret = 0;
	MutexAttr* attr = (MutexAttr*)malloc(sizeof(MutexAttr));//Malloc the struct
	
	if(pthread_mutexattr_init(attr) != 0)//Initialise the attribute struct
		ERR("init_mutexattr init error", -1)
	
	ret = (pthread_mutexattr_setpshared(attr, PTHREAD_PROCESS_SHARED));//Set the shared status -Fails and defaults to PTHREAD_PROCESS_PRIVATE-
	//printf("mutex setpshared returned %d: %s\n", ret, strerror(ret));
	
	return attr;
}

//Initialise the mutex and return a pointer to it
Mutex* init_mutex(MutexAttr* attr)
{
	Mutex* mutex = (Mutex*)malloc(sizeof(Mutex));
	if(pthread_mutex_init(mutex, attr) != 0)
		ERR("init_mutex() error", -1)
	
	return mutex;
}

//Initialise a condition attribute structure
CondAttr* init_condattr()
{
	int ret = 0;
	CondAttr* attr = (CondAttr*)malloc(sizeof(CondAttr));
	
	if(pthread_condattr_init(attr) != 0)
		ERR("init_condattr init error", -1)
	
	ret = pthread_condattr_setpshared(attr, PTHREAD_PROCESS_SHARED);
	//printf("cond setpshared returned %d: %s\n", ret, strerror(ret));
	return attr;
}

//Initialise the condition variable and return a pointer to it
Condition* init_cond(CondAttr* attr)
{
	Condition* cond = (Condition*)malloc(sizeof(Condition));
	if(pthread_cond_init(cond, attr) != 0)
		ERR("init_cond() error", -1)
	
	return cond;
}

//Wait on the semaphore using a condition variable
int wait_sem(Semaphore* sem)
{
	if(pthread_cond_wait(sem->cond, sem->mutex) != 0)
		return -1;
	return 0;
}

//Signal a thread
int signal_sem(Semaphore* sem)
{
	if(pthread_cond_signal(sem->cond) != 0)
		return -1;
	return 0;
}

//Notify all waiting threads
int broadcast_sem(Semaphore* sem)
{
	if(pthread_cond_broadcast(sem->cond) != 0)
		return -1;
	return 0;
}

//Lock the semaphore
int lock_sem(Semaphore* sem)
{
	if(pthread_mutex_lock(sem->mutex) != 0)
		return -1;
	return 0;
}

//Unlock the semaphore
int unlock_sem(Semaphore* sem)
{
	if(pthread_mutex_unlock(sem->mutex) != 0)
		return -1;
	return 0;
}
