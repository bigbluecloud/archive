/*
	Server header file
*/

#ifndef SERV_H
#define SERV_H

#include "sem.h"
#include "common.h"
#include "threadpool.h"

void runServer(int, unsigned long**, int**, int**, Semaphore*);

#endif