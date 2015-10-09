/*File that defines two functions for a better use of malloc and realloc*/
#ifndef _AUXILIARES_H_
#define _AUXILIARES_H_

#include <stdlib.h>
#include "QUEUE.h"

/**
 * Uses the same input as malloc, and has the same output, with the only 
 * difference being that it stops execution if some error occurred when
 * calling malloc.
 */
void* xmalloc(unsigned siz);

/**
 * Auxiliary function that prints the terminating info about all the processes 
 * that were correctly endend if the input parameter mode is 1. And also always frees 
 * the memory allocated for the queue, the string in argVector and the argVector itself.
 * If mode is 0 doesn't print anything.
 * Doesn't have a return value.
 */
void exitFree(char **argVector, Queue processList, int mode);

/**
 * Auxiliary function that determines if two processes are the same.
 * Two processes are the same if they have the same pid.
 * Takes as input a pointer to the pid and the pointer to a process. 
 * Returns 1 if it is the same process and 0 otherwise.
 */ 
int compareProcesses(void* pid, void* process);

#endif
