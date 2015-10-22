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
void* monitorChildProcesses(void* p){
	int status;
	pid_t child_pid;
	
	while(1){
		pthread_mutex_lock(&numChildren_lock);	
		if (numChildren > 0){
			pthread_mutex_unlock(&numChildren_lock);
			child_pid =	wait(&status);

			if (child_pid > 0){	// case the pid is valid 
				pthread_mutex_lock(&queue_lock);
				// get the process that finished from the queue
				process_info process = (process_info) getSpecificQueue(processList, &child_pid, compareProcesses, 0);

				//checks for an error on finding the element
				if (process == NULL){
					fprintf(stderr, "An error occurred when searching for a process in the list. Process not found.\n");
					pthread_mutex_unlock(&queue_lock);
					continue;
				}
				else{
					pthread_mutex_lock(&numChildren_lock);
					numChildren--;
					pthread_mutex_unlock(&numChildren_lock);
					setEndTime(process, time(NULL)); //Store the time the process terminated
					if (WIFEXITED(status))	//if the process exited store its exit status
						setExitStatus(process, WEXITSTATUS(status));	

					else	// the process didn't correctly ended, so set an error in the end time
						setPidError(process);			
				}
				pthread_mutex_unlock(&queue_lock);
			}
			else{ 
				if (errno == ECHILD){	//checks to see if the error that wait() return was because there were no child processes
					exit(EXIT_FAILURE);
				}
				else{					//if not prints the error
					fprintf(stderr, "An error occurred when wating for a process to exit. %s\n", strerror(errno));
					
					continue;
				}
			}
		}
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