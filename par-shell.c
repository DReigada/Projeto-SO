/**
 *  Projeto de SO - entrega 2
 *  data          - 22.10.15
 *  
 *  Autores       - Daniel Reigada
 *   			  - Diogo Mesquita
 *                - Sebastião Araújo
 */


// includes "standards"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>


// related to the system calls functions
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>

// related to creating threads
#include <pthread.h>

// our own files
#include "commandlinereader.h"
#include "QUEUE.h"
#include "process_info.h"
#include "Auxiliares.h"

#define NARGS 1 // number of arguments of the par-shell program 
// boolean values 
#define TRUE 1 
#define FALSE 0

#define MAX_N_INPUT 7 // the programs executed in the par-shell are limited to 
					  // 5 input arguments (the last entry is always set to NULL)
#define N_MUTEXES 3   // number of mutexes that will be needed

#define ONE_SECOND 1 
#define EXIT_COMMAND "exit"

// prototype
void* monitorChildProcesses();

// global variable to store the number of active children
int numChildren;

// global list to store the processes
Queue processList;

// global mutexes 
pthread_mutex_t queue_lock;
pthread_mutex_t numChildren_lock;
pthread_mutex_t shell_status_lock;

// global variable that is TRUE while the par-shell is running and is set to 
// False when the exit command is given
int par_shell_on;

int main(int argc, char* argv[]){

	// To initiate the par-shell no input arguments are needed
	if (argc != NARGS){
		printf("Usage: par-shell\n");
		exit(EXIT_FAILURE);
	}

	// set number of children to 0
	numChildren = 0;

	// set the par_shell to being on
	par_shell_on = TRUE;

	// init the locks
	pthread_mutex_t* mutex_list[N_MUTEXES] = {&queue_lock,
											  &numChildren_lock, 
											  &shell_status_lock};
	initMutexes(mutex_list, N_MUTEXES);

	// variables related to the monitor thread
	pthread_t thread_id;

	// start the thread and check for errors 
	int thread_err = pthread_create(&thread_id, NULL, &monitorChildProcesses, NULL);
	if (thread_err != 0){
		fprintf(stderr, "Erro ao criar a thread: %s\n", strerror(thread_err));
		exit(EXIT_FAILURE);
	}

	// allocates the memory for the command that the user inputs
	char** argVector = (char**) xmalloc(sizeof(char*) * MAX_N_INPUT); 			

	// stores the number of arguments from the user
	int narg = 0;

	// initializes the list to store the children 
	processList = initQueue();

	// Continue until the exit command is executed
	while (TRUE){
		
		// read the user input
		narg = readLineArguments(argVector, MAX_N_INPUT);

		// checks for errors reading the input and reads again if there were
		if (narg == -1){
			fprintf(stderr, "Some error occurred reading the user's input.\n");
			continue;
		}
		else if (narg == 0){ //in case no command was inputted
			fprintf(stdout, "Please input a valid command\n");
			continue;
		}

		/* see what command the user typed */

		// exit command
		if (strcmp(argVector[0], EXIT_COMMAND) == 0){

			printf("Waiting for all the processes to terminate\n");

			// gives indication to the thread to terminate
			pthread_mutex_lock (&shell_status_lock);
			par_shell_on = FALSE;
			pthread_mutex_unlock (&shell_status_lock);

			// terminates thread and destroys the locks 
			exitThread(&thread_id, mutex_list, N_MUTEXES);

			// prints the final results, terminates the thread and frees the memory allocated
			exitFree(argVector, processList, thread_id, 1);

			// exit the shell with success
			exit(EXIT_SUCCESS);
		}

		// else we assume it was given a path to a program to execute
		pid_t child_pid = fork();

		// check for errors in the creation of a new process
		if (child_pid == -1){
			fprintf(stderr, 
					"Error occurred when creating a new process: %s\n", 
					strerror(errno));
			continue;
		}

		// child executes this
		if (child_pid == 0){

			// Change the process image to the program given by the user 
			if (execv(argVector[0], argVector) < 0){ // check for errors
				//print error message
				fprintf(stderr, 
						"Error occurred when trying to open executable with the pathname: %s\n", 
						strerror(errno)); 

				// free the allocated memory that was copied for the child process
				exitFree(argVector, processList, thread_id, 0);

				exit(EXIT_FAILURE); //exits
			}
		}

		// parent executes this
		else{
			process_info process = createProcessInfo(child_pid, time(NULL));

			//add the created process to the list and increment the number of children
			pthread_mutex_lock(&numChildren_lock);
			pthread_mutex_lock(&queue_lock);

			numChildren++;
			addQueue(process, processList);

			pthread_mutex_unlock(&queue_lock);
			pthread_mutex_unlock(&numChildren_lock);

			//free the memory allocated to store new commands
			free(argVector[0]);
		}
	}
}

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



