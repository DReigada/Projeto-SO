/*File that defines two functions for a better use of malloc and realloc*/
#include "Auxiliares.h"
#include "process_info.h"
#include <stdio.h>

/**
 * Uses the same input as malloc, and has the same output, with the only 
 * difference being that it stops execution if some error occurred when
 * calling malloc.
 */
  void* xmalloc(unsigned siz){
    void* mem = malloc(siz); /* allocate the needed memory */

	 if (mem == NULL){ fprintf(stderr, "No memory in line %d of file \"%s\".\n",
	   	 __LINE__, __FILE__); exit(2);} /* check for errors in allocating memory */

		  return mem;
  }

/**
 * Auxiliary function that prints the terminating info about all the processes 
 * that were correctly endend if the input parameter mode is 1. And also always frees 
 * the memory allocated for the queue, the string in argVector and the argVector itself.
 * If mode is 0 doesn't print anything.
 * Doesn't have a return value.
 */
void exitFree(char **argVector, Queue processList, int mode) {

	while (!isEmptyQueue (processList)) { //while the list is not empty print the info of all processes

		process_info process = (process_info) getFirstQueue(processList);

		if (mode && (getPid(process) != -1))
			fprintf(stdout, "Process %d terminated with status %d\n", getPid(process), getExitStatus(process));

		freeProcInfo(process);		//free the process info struct
	}

	// free memory
	freeQ(processList);
	free(argVector[0]);
	free(argVector);
}

/**
 * Auxiliary function that determines if two processes are the same.
 * Two processes are the same if they have the same pid.
 * Takes as input a pointer to the pid and the pointer to a process. 
 * Returns 1 if it is the same process and 0 otherwise.
 */ 
int compareProcesses(void* pid, void* process){ 

	return *(pid_t*) pid ==  getPid((process_info) process);

}

