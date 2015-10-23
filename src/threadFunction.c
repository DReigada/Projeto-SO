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
 * Monitors the active children processes. Waits for them to finish and
 * when they have it terminates them and sets their end time
 * 
 * @param arg is it's sole argument
 * Returns 0 if exited with no errors and -1 otherwise
 */
void* monitorChildProcesses(){
	int status;
	pid_t child_pid;
	
	// monitor thread indefinately looking for children or for the exit command 
	while(1){

		pthread_mutex_lock(&numChildren_lock);	

		// if there are active children processes wait for them to finish
		if (numChildren > 0){
			pthread_mutex_unlock(&numChildren_lock);
			child_pid =	wait(&status);

			if (child_pid > 0){	// case the pid is valid 
				pthread_mutex_lock(&queue_lock);
				// get the process that finished from the queue
				process_info process = (process_info) 
										getSpecificQueue(processList, 
														 &child_pid, 
														 compareProcesses, 
														 0);
				pthread_mutex_unlock(&queue_lock);

				//checks for an error on finding the element
				if (process == NULL){
					fprintf(stderr, "An error occurred when searching for a "
							"process in the list. Process not found.\n");
					continue;
				}
				else{
					// decrease the number of active children
					pthread_mutex_lock(&numChildren_lock);
					numChildren--;
					pthread_mutex_unlock(&numChildren_lock);

					//Store the time the process terminated
					setEndTime(process, time(NULL)); 

					//if the process exited store its exit status
					if (WIFEXITED(status))	
						setExitStatus(process, WEXITSTATUS(status));	

					// the process didn't exit, so set an error in the end time
					else	
						setExitError(process);			
				}
			}
			else{ 
				//if the error was because there were no child processes
				if (errno == ECHILD){
					fprintf(stderr, "The child disappeared: %s\n", strerror(errno));	
					exit(EXIT_FAILURE);
				}
				//if not prints the error
				else{					
					fprintf(stderr, 
							"An error occurred when wating for a process to exit. %s\n", 
							strerror(errno));
					
					continue;
				}
			}
		}
		// check if the exit command was given
		else if(!par_shell_on){
			pthread_mutex_unlock(&numChildren_lock); 
			break;
		}
		else{
			pthread_mutex_unlock(&numChildren_lock);
			sleep(1);
		}
	}
	return NULL;
}