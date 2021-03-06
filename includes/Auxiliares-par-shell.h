/*File that defines auxiliary functions used in par-shell*/
#ifndef _AUXILIARES_PARSHELL_H_
#define _AUXILIARES_PARSHELL_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "process_info.h"
#include "QUEUE.h"
#include <Message.h>

// boolean values
#define TRUE 1
#define FALSE 0

#ifdef DEFINE_VARIABLES
#define EXTERN /* nothing */
#else
#define EXTERN extern
#endif

// the flag for the SIGINT handler
EXTERN int sigintFlag;

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
 * Frees the memory allocated for the processes queue, the string
 * in argVector, the argVector itself and the terminalsList.
 *
 * If mode is 1 it also prints the terminating info about all the processes
 * that were correctly endend.
 *
 * Doesn't have a return value.
 */
void exitFree (char **argVector, Queue processList, int mode, Queue terminalsList);

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
 * Reads the number of total iterarions and total execution time from log file.
 * Takes as inputs two pointers to integers to store the values and the log file
 */
 int readLog(int *iterationsNumber, int *executionTime, FILE *log);

/**
 * Writes to the log file the data of a terminated process
 * and updates the interation number and total execution time
 * Takes as inputs two pointers to the interation number and execution time,
 * a pointer to the process and the log file
 */
void writeLog(int *iterationNum, int *execTime, process_info process, FILE *log);

/**
 * Tests if the given strings match the format specified for the log file lines
 * Returns 0 if they dont match
 */
int testlines(char *iteration, char *pid, char *exectime);

/**
 * Counts the number of tokens delimited by delim chars in a string
 * Returns the number of tokens
 */
int countTokens(char *str, const char *delim);

/**
 * Compares two pid_t referenced by pid1 and pid2.
 * Returns 0 if they are not equal.
 */
int comparePids(void *pid1, void *pid2);

/**
 * Kills all the terminals in the given list,
 * except the one with pid equal to callingPid
 */
void killTerminals(Queue terminalsList, pid_t callingPid);

/**
 * The handler for SIGINT
 */
void sigintHandler(int signal);

/**
 * Reads a Message from stdin and stores it in the memory pointed by message
 * Returns:
 * 		- 0 on success;
 *   	- 1 if it read EOF;
 *   	- 2 if read was interrupted by a signal before any data was read (EINTR);
 *   	- -1 on error reading.
 */
int readMessage(Message *message);

#endif
