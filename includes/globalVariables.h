/**
 * File that defines the shared variables to be used by
 * the par-shell and the monitor thread
 */
#ifndef _GLOBALVARIABLES_H_
#define _GLOBALVARIABLES_H_

#ifdef DEFINE_VARIABLES
#define EXTERN /* nothing */
#else
#define EXTERN extern
#endif

#include <stdio.h>

// related to creating and managing threads
#include <pthread.h>


#include "QUEUE.h"

// global variable to store the number of active children
EXTERN int numChildren;

// global list to store the processes
EXTERN Queue processList;

// global mutexes
EXTERN pthread_mutex_t queue_lock;
EXTERN pthread_mutex_t numChildren_lock;

// global condition variables
EXTERN pthread_cond_t numChildren_cond_variable;

// global variable that is TRUE while the par-shell is running and is set to
// False when the exit command is given
EXTERN int par_shell_on;

// the file to store the information about the processes
EXTERN FILE *logFile;



#endif
