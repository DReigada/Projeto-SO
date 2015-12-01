/**
 *  Projeto de SO - entrega 3
 *  data          - 30.10.15
 *
 *  Autores       - Daniel Reigada
 *   			  			- Diogo Mesquita
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

// related to the system calls functions and files functions
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// our own files
#include "commandlinereader.h"
#include "process_info.h"
#include "Auxiliares.h"
#include "Auxiliares-par-shell.h"

// Constants
#define NARGS 1 // number of arguments of the par-shell program
#define MAX_N_INPUT 7  // the programs executed in the par-shell are limited to
					   // 5 input arguments (the last entry is always set to NULL)

#define N_MUTEXES 2  // number of mutexes that will be needed
#define MAXPAR 4	 // maximum number of child processes in any given moment

#define EXIT_COMMAND "exit"

// define the fopen modes
#define READAPPEND "a+"

// the location of the log file
#define LOGFILE "log.txt"

// filename format of the created processes output, its max size and permissions
#define OUTPUT_FILE_FORMAT "par-shell-out-%d.txt"
#define OUTPUT_NAME_MAX_SIZE 50
#define PERMISSIONS 0666

// the name of the FIFO for inputs
#define FIFO_NAME "par-shell-in"

// the messages from the terminals
#define START_MESSAGE "start"
#define EXIT_MESSAGE "exit"
#define EXIT_GLOBAL_MESSAGE "exit-global"

int main(int argc, char* argv[]){

	// To initiate the par-shell no input arguments are needed
	if (argc != NARGS){
		printf("Usage: par-shell\n");
		exit(EXIT_FAILURE);
	}

	// close the stdin and open the FIFO in its place
	xclose(0);
	xunlink(FIFO_NAME);
	xmkfifo(FIFO_NAME, PERMISSIONS); //TODO xmkfifo
	xopen(FIFO_NAME, O_RDONLY | O_CREAT, PERMISSIONS);

	// terminals related variables
	int numTerminals = 0;
	Queue terminalsList = initQueue();

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
	argVector[0] = NULL;
	// stores the number of arguments from the user
	int narg = 0;

	// initializes the list to store the children
	processList = initQueue();

	// Continue until the exit command is executed
	while (TRUE){

		//free the memory allocated to store new commands
		free(argVector[0]);

		// read the user input
		narg = readLineArguments(argVector, MAX_N_INPUT);

		// check for errors reading the input and read again if there were any
		if (narg == -1){
			fprintf(stderr, "Some error occurred reading the user's input.\n");
			continue;
		}
		// in case no command was inserted
		else if (narg == 0){
			fprintf(stdout, "Please input a valid command\n");
			continue;
		}
		// in case read returned EOF and there were active terminals
		if (narg == -2 && numTerminals > 0) {
			numTerminals = 0;
			// wait for a terminal to open the pipe
			// TODO see the best way to do this
			xclose(xopen(FIFO_NAME, O_RDONLY, PERMISSIONS));
			continue;
		}

		// case it is a special message from a terminal
		if (strcmp(argVector[0], "\a") == 0){

			// case its the start message
			if ((strcmp(argVector[1], START_MESSAGE) == 0)) {
				pid_t *pid = malloc(sizeof(pid_t));
				*pid = atoi(argVector[2]);
				addQueue(pid, terminalsList);
			  numTerminals++;
			  continue;
			}

			// case it is the exit message
			if (strcmp(argVector[1], EXIT_MESSAGE) == 0){
					if (--numTerminals == 0)
					// wait for a terminal to open the pipe
					// TODO see the best way to do this
						xclose(xopen(FIFO_NAME, O_RDONLY, PERMISSIONS));
				continue;
			}

			// case it is the exit-global message
			if (strcmp(argVector[1], EXIT_GLOBAL_MESSAGE) == 0) {
				pid_t *pid;
				while((pid = getFirstQueue(terminalsList)) != NULL){
					printf("Killing terminal with pid %d\n", *pid);
					kill(*pid, SIGTERM); // TODO check errors
					free(pid);
				}
				break;
			}

			printf("Invalid message from a terminal\n");
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

			// close the stdout and open the output file in its place
			xclose(1);
			char filename[OUTPUT_NAME_MAX_SIZE];
			sprintf(filename, OUTPUT_FILE_FORMAT, getpid());
			xopen(filename, O_WRONLY | O_CREAT, PERMISSIONS);

			// Change the process image to the program given by the user
			if (execv(argVector[0], argVector) < 0){ // check for errors
				fprintf(stderr,
						"Error occurred when trying to open the executable with "
						"the pathname: %s\n",
						strerror(errno));

				// case there was an error calling execv

				// free the allocated memory that was copied for the child process
				exitFree(argVector, processList, 0, terminalsList);

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
	exitFree(argVector, processList, 1, terminalsList);

	// close the log file
	xfclose(logFile);

	// exit the shell with success
	exit(EXIT_SUCCESS);
}
