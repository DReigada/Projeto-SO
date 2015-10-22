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

#define ONE_SECOND 1 

/**
 * Monitors the active children processes. Waits for them to finish and
 * when they have it terminates them and sets their end time
 * 
 * @param arg is it's sole argument
 * Returns 0 if exited with no errors and -1 otherwise
 */
 void* monitorChildProcesses(){

 	// initialize the process struct
	process_info process;

	// variables to store the child info 
	int status;
	pid_t child_pid;

	// repeat this while the user didn't exit the par-shell or there are still active children
	while (par_shell_on || numChildren){

		// repeat until there are no more active children
		while (numChildren){

			child_pid = wait(&status);

			if (child_pid > 0){	// case the pid is valid 

				// get the process that finished from the queue
				pthread_mutex_lock(&queue_lock);
				process = (process_info) getSpecificQueue(processList, &child_pid, compareProcesses, 0);
				pthread_mutex_unlock(&queue_lock);

				//checks for an error on finding the element
				if (process == NULL){
					fprintf(stderr, "An error occurred when searching for a process in the list. Process not found.\n");
					continue;
				}
				else{
					setEndTime(process, time(NULL)); //Setting the end time

					if (WIFEXITED(status))
						//if the process exited store its exit status
						setExitStatus(process, WEXITSTATUS(status));
						

					else	// the process didn't correctly ended, so set an error in the end time
						setPidError(process);

					//decrement the number of children
					pthread_mutex_lock(&numChildren_lock);
					numChildren--;	
					pthread_mutex_unlock(&numChildren_lock);
				}
			}
			else{ 
				fprintf(stderr, "An error occurred when wating for a process to exit. %s\n", strerror(errno));
				break;
			}
		}

		sleep(ONE_SECOND);
	}

	// returns 
	pthread_exit(NULL);

 }






