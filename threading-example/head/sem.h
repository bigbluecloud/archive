/*
	Semaphore header
*/

#ifndef SEM_H
#define SEM_H

#include "common.h"

//Mutex & conditon variable typedef
typedef pthread_mutex_t Mutex;
typedef pthread_cond_t Condition;

//Attribute typedefs
typedef pthread_mutexattr_t MutexAttr;
typedef pthread_condattr_t CondAttr;


//Semaphore structure
typedef struct semaphores
{
	Mutex* mutex;			//Used to lock-unlock critical sections
	Condition* cond;		//Used to wait on signals
	MutexAttr* mutexattr;	//For setting the pshared attribute of the mutex
	CondAttr* condattr;		//For setting the pshared attribute of the condition variable
}Semaphore;

//Function prototypes
Semaphore* create_semaphore();
int destroy_semaphore(Semaphore*);

Condition* init_cond(CondAttr*);
Mutex* init_mutex(MutexAttr*);

CondAttr* init_condattr();
MutexAttr* init_mutexattr();

int wait_sem(Semaphore*);
int signal_sem(Semaphore*);
int broadcast_sem(Semaphore*);

int lock_sem(Semaphore*);
int unlock_sem(Semaphore*);

#endif //SEM_H
