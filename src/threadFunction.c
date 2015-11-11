/* Defines the function that the monitor thread for the par-shell is going to run */

#include "threadFunction.h"

#include "globalVariables.h"   // global variables shared with the main program

// own funtions
#include "QUEUE.h"
#include "process_info.h"
#include "Auxiliares.h"

// GNU C libraries
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

// related to the system calls functions
#include <sys/wait.h>
#include <sys/types.h>

#define ONE_SECOND 1

/**
 * Monitors the active children processes. Waits for them to finish and,
 * when they have, terminates them and sets their end time
 */
void* monitorChildProcesses(){
	int status;
	pid_t child_pid;

	// indefinitely looking for children or for the exit command
	while(1){

		// wait for a child process to be created or the exit command
		mutex_lock(&numChildren_lock);
		while (par_shell_on && numChildren < 1)
			xcond_wait(&numChildren_cond_variable, &numChildren_lock);
		mutex_unlock(&numChildren_lock);

		// check if the exit command was given and there are no more child processes
		if(!par_shell_on && numChildren < 1)
			break;

		/* here we know there is at least 1 active children process */

		// wait for it to finish
		child_pid =	wait(&status);

		if (child_pid > 0){	// case the childs pid is valid
			mutex_lock(&queue_lock);

			// get the process that finished from the queue
			process_info process = (process_info)
									getSpecificQueue(processList,
													 &child_pid,
													 compareProcesses,
													 0);
			mutex_unlock(&queue_lock);

			// check for an error on finding the element
			if (process == NULL){
				fprintf(stderr, "An error occurred when searching for a "
						"process in the list. Process not found.\n");
				continue;
			}
			else{
				// decrease the number of active children
				mutex_lock(&numChildren_lock);
				numChildren--;
				mutex_unlock(&numChildren_lock);

				// allow par-shell to create more processes
				xcond_signal(&numChildren_cond_variable);

				// store the necessary info
				updateTerminatedProcess(process, time(NULL), status);

				// writes the process data to the log file
				 writeLog(&iterationNum, &execTime, process, logFile);

			}
		}
		else{   // gets the error
			fprintf(stderr,
					"An error occurred when wating for a process to exit. %s\n",
					strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	pthread_exit(NULL);
}
