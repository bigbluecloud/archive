/*
	As Cygwin does not support barriers, a basic implementation found at http://www.howforge.com/implementing-barrier-in-pthreads was used
	The following code is credited to user 'surgee' of howforge.com
*/

#ifndef CYGBARRIER
#define CYGBARRIER

#include <stdlib.h>

#include "common.h"

#ifndef WIN32
#include <sys/types.h>
#endif

#if defined(WITH_BARRIER) || defined(CYGWIN)
#define pthread_barrier_t barrier_t
#define pthread_barrier_attr_t barrier_attr_t
#define pthread_barrier_init(b,a,n) barrier_init(b,n)
#define pthread_barrier_destroy(b) barrier_destroy(b)
#define pthread_barrier_wait(b) barrier_wait(b)
#endif

typedef struct {
    int needed;					//Number of threads needed to join the barrier
    int called;					//Number of threads at the barrier
    pthread_mutex_t mutex;		//Used to block/release the threads at the barrier
    pthread_cond_t cond;		//Used to signal the threads at the barrier
} barrier_t;

int barrier_init(barrier_t *barrier,int needed);
int barrier_destroy(barrier_t *barrier);
int barrier_wait(barrier_t *barrier);

#endif//CYGBARRIER
