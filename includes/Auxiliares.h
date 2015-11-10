/*File that defines two functions for a better use of malloc and realloc*/
#ifndef _AUXILIARES_H_
#define _AUXILIARES_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <process_info.h>

#include "QUEUE.h"

/**
 * Uses the same input as malloc, and has the same output, with the only
 * difference being that it stops execution if some error occurred when
 * calling malloc.
 */
void* xmalloc(unsigned siz);

/**
 * Uses the same input as pthread_mutex_lock (and no output), with the only
 * difference being that it stops execution if some error occurred when
 * calling pthread_mutex_lock.
 */
void mutex_lock(pthread_mutex_t* lock);

/**
 * Uses the same input as pthread_mutex_unlock (and no output), with the only
 * difference being that it stops execution if some error occurred when
 * calling pthread_mutex_unlock.
 */
void mutex_unlock(pthread_mutex_t* lock);

/**
 * Uses the same input as pthread_cond_init (and no output), with the only
 * difference being that it stops execution if some error occurred when
 * calling pthread_cond_init.
 */
void xcond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);

/**
 * Uses the same input as pthread_cond_destroy (and no output), with the only
 * difference being that it stops execution if some error occurred when
 * calling pthread_cond_destroy.
 */
void xcond_destroy(pthread_cond_t *cond);

/**
 * Uses the same input as pthread_cond_wait (and no output), with the only
 * difference being that it stops execution if some error occurred when
 * calling pthread_cond_wait.
 */
void xcond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);

/**
 * Uses the same input as pthread_cond_signal (and no output), with the only
 * difference being that it stops execution if some error occurred when
 * calling pthread_cond_signal.
 */
void xcond_signal(pthread_cond_t *cond);

/**
 * Frees the memory allocated for the queue, the string
 * in argVector and the argVector itself.
 *
 * If mode is 1 it also prints the terminating info about all the processes
 * that were correctly endend.
 *
 * Doesn't have a return value.
 */
void exitFree(char **argVector, Queue processList, int mode);

/**
 * Initializes the thread and the mutexes.
 *
 * thread_id is a pointer to a thread id.
 * Upon creation of the thread, the thread executes start_routine - start_routine
 * must not take any input arguments.
 *
 * mutex_id_list is a list of length n_mutexes of pointers to the mutexes' ids.
 */
void initThread (pthread_t* thread_id,
				 void* (*start_routine)(void*),
				 pthread_mutex_t* mutex_id_list[],
				 int n_mutexes);

/**
 * Terminates the monitor thread and destroys the locks.
 *
 * thread_id is the pointer to the thread's id to be terminated.
 * mutex_id_list is a list of length n_mutexes of pointers to the mutexes' ids.
 */
void exitThread (pthread_t* thread_id, pthread_mutex_t* mutex_id_list[], int n_mutexes);

/**
 * Updates everything needed once a process terminates.
 *
 * Needs as inputs the process, it's end time and the status with which it ended.
 * It doesn't return anything.
 */
void updateTerminatedProcess (process_info process, time_t end_time, int status);

/**
 * Auxiliary function that determines if two processes are the same.
 * Two processes are the same if they have the same pid.
 * Takes as input a pointer to the pid and the pointer to a process.
 * Returns 1 if it is the same process and 0 otherwise.
 */
int compareProcesses(void* pid, void* process);

/**
 * Uses the same input as fopen but if some error occurs when calling fopen
 * it does not return null, instead it stops the execution.
 */
 FILE *xfopen(const char *path, const char *mode);

#endif
