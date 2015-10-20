/*File that defines two functions for a better use of malloc and realloc*/
#include "Auxiliares.h"
#include "process_info.h"
#include <stdio.h>
#include <string.h>

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
 * Auxiliary function that frees the memory allocated for the queue, the string 
 * in argVector and the argVector itself. 
 *
 * If mode is 1 it also prints the terminating info about all the processes
 * that were correctly endend and terminates the monitor thread.
 *
 * Doesn't have a return value.
 */
void exitFree(char **argVector, Queue processList, pthread_t thread_id, int mode) {

	// terminate monitor thread if mode is 1
	if (mode){

		// waits for thread to terminate and when it does checks for errors
		int terminate_thread = pthread_join(thread_id, NULL);
		if (terminate_thread != 0)
			fprintf(stderr, "Error terminating monitor thread: %s\n", strerror(terminate_thread));

		// print info about the terminated thread
		else {
			printf("Monitor thread: %d terminated normally.\n", (int) thread_id);
		}
	}

	while (!isEmptyQueue (processList)) { //while the list is not empty print the info of all processes
		process_info process = (process_info) getFirstQueue(processList);

		if (mode && (getPid(process) != -1))
			fprintf(stdout,
				"Process %d terminated normally: status=%d | execution time: %ld \n",
				getPid(process), 
				getExitStatus(process), 
				getEndTime(process) - getStartTime(process));

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

