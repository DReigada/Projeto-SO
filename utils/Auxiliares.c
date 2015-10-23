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
  void* xmalloc (unsigned siz){
    void* mem = malloc(siz); /* allocate the needed memory */

	 if (mem == NULL){ fprintf(stderr, "No memory in line %d of file \"%s\".\n",
	   	 __LINE__, __FILE__); exit(2);} /* check for errors in allocating memory */

		  return mem;
  }

/**
 * Frees the memory allocated for the queue, the string 
 * in argVector and the argVector itself. 
 *
 * If mode is 1 it also prints the terminating info about all the processes
 * that were correctly endend.
 *
 * Doesn't have a return value.
 */
void exitFree (char **argVector, Queue processList, pthread_t thread_id, int mode) {

	//while the list is not empty print the info of all processes
	while (!isEmptyQueue (processList)) { 
		process_info process = (process_info) getFirstQueue(processList);

		// print the processes that terminated normally
		if (mode && exitedCorrectly(process))
			fprintf(stdout,
				"Process %d terminated normally: status=%d | execution time: %ld \n",
				getPid(process), 
				getExitStatus(process), 
				getEndTime(process) - getStartTime(process));
		// print the ones that didn't terminate normally
		else if (mode)
			fprintf(stdout, 
					"Process %d terminated without calling exit.\n", 
					getPid(process));
		freeProcInfo(process);		//free the process info struct
	}

	// free memory
	freeQ(processList);
	free(argVector[0]);
	free(argVector);
}

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
				 int n_mutexes) {

	// init the locks
	int lock_err, i;
	for (i = 0; i < n_mutexes; i++){

		if ((lock_err = pthread_mutex_init(mutex_id_list[i], NULL)) != 0)
			fprintf(stderr, 
					"Error creating the lock number %d: %s\n", 
					i+1, strerror(lock_err));
	}
	
	// init the thread 
	int thread_err;
	if ((thread_err = pthread_create(thread_id, NULL, start_routine, NULL)) != 0){
		fprintf(stderr, "Erro ao criar a thread: %s\n", strerror(thread_err));
		exit(EXIT_FAILURE);
	}		
}

/**
 * Terminates the monitor thread and destroys the locks.
 *
 * thread_id is the pointer to the thread's id to be terminated.
 * mutex_id_list is a list of length n_mutexes of pointers to the mutexes' ids.
 */
void exitThread (pthread_t* thread_id, pthread_mutex_t* mutex_id_list[], int n_mutexes){

	// waits for thread to terminate and when it does checks for errors
	int thread_err;
	if ((thread_err = pthread_join(*thread_id, NULL)) != 0)
		fprintf(stderr, 
				"Error terminating thread: %s\n", 
				strerror(thread_err));

	// Destroys the locks 
	int lock_err, i;
	for (i = 0; i < n_mutexes; i++){
		if ((lock_err = pthread_mutex_destroy(mutex_id_list[i])) != 0)
			fprintf(stderr, "Error destroying the lock number %d: %s\n", 
					i+1, strerror(lock_err));
	}
}

/**
 * Determines if two processes are the same.
 * Two processes are the same if they have the same pid.
 * Takes as input a pointer to the pid and the pointer to a process. 
 * Returns 1 if it is the same process and 0 otherwise.
 */ 
int compareProcesses (void* pid, void* process){ 

	return *(pid_t*) pid ==  getPid((process_info) process);

}

