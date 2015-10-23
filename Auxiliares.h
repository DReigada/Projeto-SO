/*File that defines two functions for a better use of malloc and realloc*/
#ifndef _AUXILIARES_H_
#define _AUXILIARES_H_

#include <stdlib.h>
#include "QUEUE.h"
#include <pthread.h>

/**
 * Uses the same input as malloc, and has the same output, with the only 
 * difference being that it stops execution if some error occurred when
 * calling malloc.
 */
void* xmalloc(unsigned siz);

/**
 * Frees the memory allocated for the queue, the string 
 * in argVector and the argVector itself. 
 *
 * If mode is 1 it also prints the terminating info about all the processes
 * that were correctly endend.
 *
 * Doesn't have a return value.
 */
void exitFree(char **argVector, Queue processList, pthread_t thread_id, int mode);

/**
 * Initializes the thread and the mutexes. 
 *
 * thread_id is a pointer to a thread id.
 * Upon creation of the thread, the thread executes start_routine - start_routine 
 * must not take any input arguments.
 *
 * mutex_id_list is a list of length n_mutexes of pointers to the mutexes' ids.
 */
void initThread (pthread_t* thread_id, void* (*start_routine)(void*), pthread_mutex_t* mutex_id_list[], int n_mutexes);

/**
 * Terminates the monitor thread and destroys the locks.
 *
 * thread_id is the pointer to the thread's id to be terminated.
 * mutex_id_list is a list of length n_mutexes of pointers to the mutexes' ids.
 */
void exitThread (pthread_t* thread_id, pthread_mutex_t* mutex_id_list[], int n_mutexes);

/**
 * Auxiliary function that determines if two processes are the same.
 * Two processes are the same if they have the same pid.
 * Takes as input a pointer to the pid and the pointer to a process. 
 * Returns 1 if it is the same process and 0 otherwise.
 */ 
int compareProcesses(void* pid, void* process);

#endif
