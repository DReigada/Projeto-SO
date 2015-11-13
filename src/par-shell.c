/**
 *  Projeto de SO - entrega 3
 *  data          - 30.10.15
 *
 *  Autores       - Daniel Reigada
 *   			  - Diogo Mesquita
 *                - Sebastião Araújo
 */
#define DEFINE_VARIABLES //indicate that we are defining the global variables

// shared variables and the monitor function
#include "globalVariables.h"
#include "threadFunction.h"

// GNU C libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>

// related to the system calls functions
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>

// our own files
#include "commandlinereader.h"
#include "process_info.h"
#include "Auxiliares.h"

// Constants
#define NARGS 1 // number of arguments of the par-shell program
#define MAX_N_INPUT 7  // the programs executed in the par-shell are limited to
					   // 5 input arguments (the last entry is always set to NULL)

#define N_MUTEXES 2  // number of mutexes that will be needed
#define MAXPAR 4	 // maximum number of child processes in any given moment

// define the fopen modes
#define READAPPEND "a+"

#define LOGFILE "log.txt" // the location of the log file

#define EXIT_COMMAND "exit"

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

	// open and read data from logFile
	logFile = xfopen(LOGFILE, READAPPEND);
	// if the file is corrupted delete its content
	if(readLog(&iterationNum, &execTime, logFile) == 0)
		logFile = xfreopen(LOGFILE, "w+", logFile);


	// initialize the condition variable for the number of child processes
	xcond_init(&numChildren_cond_variable, NULL);

	// declare variable to store the thread id
	pthread_t thread_id;

	// init the locks and the thread
	pthread_mutex_t* mutex_list[N_MUTEXES] = {&queue_lock, &numChildren_lock};

	initThread(&thread_id, &monitorChildProcesses, mutex_list, N_MUTEXES);

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

		// check for errors reading the input and read again if there were any
		if (narg == -1){
			fprintf(stderr, "Some error occurred reading the user's input.\n");
			continue;
		}
		else if (narg == 0){ // in case no command was inserted
			fprintf(stdout, "Please input a valid command\n");
			continue;
		}

		// case the command is exit
		if (strcmp(argVector[0], EXIT_COMMAND) == 0){
			printf("Waiting for all the processes to terminate\n");
			break;
		}

		/* else we assume it was given a path to a program to execute */

		// wait if the limit of childs was reached
		mutex_lock(&numChildren_lock);
		while (numChildren >= MAXPAR)
			xcond_wait(&numChildren_cond_variable, &numChildren_lock);
		mutex_unlock(&numChildren_lock);


		pid_t child_pid = fork();    // create a new child process
		if (child_pid == -1){        // check for errors
			fprintf(stderr,
					"Error occurred when creating a new process: %s\n",
					strerror(errno));
			continue;
		}
		if (child_pid == 0){ 		 // child executes this

			// Change the process image to the program given by the user
			if (execv(argVector[0], argVector) < 0){ // check for errors
				fprintf(stderr,
						"Error occurred when trying to open the executable with "
						"the pathname: %s\n",
						strerror(errno));

				// case there was an error calling execv

				// free the allocated memory that was copied for the child process
				exitFree(argVector, processList, 0);

				// close the log file
				xfclose(logFile);

				exit(EXIT_FAILURE); //exits
			}
		}
		else{		// parent executes this
			process_info process = createProcessInfo(child_pid, time(NULL));

			// add created process to the list and increment number of children
			mutex_lock(&numChildren_lock);
			mutex_lock(&queue_lock);

			numChildren++;
			addQueue(process, processList);

			mutex_unlock(&queue_lock);
			mutex_unlock(&numChildren_lock);

			// allow monitor thread to run (unblock it)
			xcond_signal(&numChildren_cond_variable);

			//free the memory allocated to store new commands
			free(argVector[0]);
		}
	}
	/* exit command was given */

	// indicate the thread to terminate
	par_shell_on = FALSE;

	// unlock monitor thread from waiting for child and exit
	xcond_signal(&numChildren_cond_variable);

	// terminate thread and destroy the locks
	exitThread(&thread_id, mutex_list, N_MUTEXES);

	xcond_destroy(&numChildren_cond_variable);

	// print final info and free allocated memory
	exitFree(argVector, processList, 1);

	// close the log file
	xfclose(logFile);

	// exit the shell with success
	exit(EXIT_SUCCESS);
}
