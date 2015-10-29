/** 
 * File that defines the shared variables to be used by 
 * the par-shell and the monitor thread 
 */
#ifndef _GLOBALVARIABLES_H_
#define _GLOBALVARIABLES_H_

#include <pthread.h>
#include "QUEUE.h"

#ifdef DEFINE_VARIABLES
#define EXTERN /* nothing */
#else
#define EXTERN extern
#endif 

// related to creating threads
#include <pthread.h>
#include <semaphore.h>

#include "QUEUE.h"

// global variable to store the number of active children
EXTERN int numChildren;

// global list to store the processes
EXTERN Queue processList;

// global mutexes 
EXTERN pthread_mutex_t queue_lock;
EXTERN pthread_mutex_t numChildren_lock;

//global semaphores
EXTERN sem_t children_sem;  // indicates the number of active children in any given moment

// global variable that is TRUE while the par-shell is running and is set to 
// False when the exit command is given
EXTERN int par_shell_on;



#endif

