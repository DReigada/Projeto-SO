/*File that defines two functions for a better use of malloc and realloc*/
#ifndef _AUXILIARES_H_
#define _AUXILIARES_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <process_info.h>

#include "QUEUE.h"

// boolean values
#define TRUE 1
#define FALSE 0

//general conventions used
#define READ_WRITE_ALL 0666
#define READ_WRITE_EXEC_ALL 0777
#define READAPPEND "a+"

#define MAX_PID_SIZE 20

// exit command
#define EXIT_COMMAND "exit"

#define SZ BUFSIZ + MAX_PID_SIZE

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
 * Frees the memory allocated for the queue and the argVector.
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

/**
 * Uses the same input as freopen but if some error occurs when calling freopen
 * it does not return null, instead it stops the execution.
 */
FILE *xfreopen(const char *path, const char *mode, FILE *stream);

 /**
  * Uses the same input as fclose (and no output), with the only
  * difference being that it stops execution if some error occurred when
  * calling fclose.
  */
void xfclose(FILE *fp);

/**
 * Uses the same input as fflush (and no output), with the only
 * difference being that it stops execution if some error occurred when
 * calling flush.
 */
void xfflush(FILE *stream);

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
 * Uses the same input as open but if some error occurs when calling open
 * it does not return null, instead it stops the execution.
 */
int xopen(const char *pathname, int flags, mode_t mode);

/**
 * Uses the same input as close (and no output), with the only
 * difference being that it stops execution if some error occurred when
 * calling close.
 */
void xclose(int fd);

/**
 * Uses the same input as mkfifo (and no output), with the only
 * difference being that it stops execution if some error occurred when
 * calling mkfifo.
 */
void x_mkfifo(const char *path, mode_t mode);

/**
 * Uses the same input as the write function but if some error occurs when
 * calling write it does not return null, instead it stops the execution.
 * Returns the number of bytes written.
 */
ssize_t xwrite(int fildes, const void *buf, size_t nbyte);

/**
 * Uses the same input as the read function but if some error occurs when
 * calling read it does not return null, instead it stops the execution.
 * Returns the number of bytes read.
 */
ssize_t xread(int fildes, void *buf, size_t nbyte);

/**
 * Parses the first message sent by a terminal.
 * This message follows the following format: '\apid command'. Where pid
 * is the pid of the terminal's process.
 * 
 * Takes as input buf, which contains the original message and an empty string
 * command where the function will store the command.
 *
 * Returns the retrieved pid from the message and stores in 'commmand' the 
 * retrieved command from the message. Also modifies buf to be an empty string.
 */
pid_t parseCommandWithPid(char* buf, char command[]);

#endif


