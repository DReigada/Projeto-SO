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

// related to the system calls functions and files functions
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// our own files
#include "parseInput.h"
#include "process_info.h"
#include "Auxiliares.h"

// Constants
#define NARGS 1 // number of arguments of the par-shell program
#define MAX_N_INPUT 7  // the programs executed in the par-shell are limited to
					   // 5 input arguments (the last entry is always set to NULL)

#define N_MUTEXES 2  // number of mutexes that will be needed
#define MAXPAR 4	 // maximum number of child processes in any given moment

// par-shell's fifo (where to send the commands)
#define PARSHELL_IN_FIFO "par-shell-in"

// exit the par-shell closing all remote terminals and open processes in order
#define GLOBAL_EXIT_COMMAND "exit-global"

// the location of the log file
#define LOGFILE "log.txt"

// filename format of the created processes output, its max size and permissions
#define OUTPUT_FILE_FORMAT "par-shell-out-%d.txt"
#define OUTPUT_NAME_MAX_SIZE 50


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

	// variables to store the output of getArguments and xread respectively
	int narg;
	int n;

	// buffer to store the command that is sent via the fifo
	char* buf = (char*) xmalloc(sizeof(char*) * SZ);
	char command[SZ];

	// initializes the list to store the children
	processList = initQueue();

	// initializes the list to store the pids of the active remote terminals
	Queue activeRemoteList = initQueue();

	// open named pipe
	x_mkfifo(PARSHELL_IN_FIFO, READ_WRITE_EXEC_ALL);
	int parshell_fifo = xopen(PARSHELL_IN_FIFO, O_RDONLY, READ_WRITE_EXEC_ALL);

	// Continue until the exit command is executed
	while (TRUE){

		// read the message from the fifo - if it's empty dont do anything
		if ((n = xread(parshell_fifo, buf, BUFSIZ)) == 0) continue; 

		// first character is '\a', then its a new terminal or a terminating one
		if (buf[0] == '\a'){  		

			// parse the message with the terminal pid accordingly
			pid_t remote_pid = parseCommandWithPid(buf, command);

			//remove the remote terminal if it exited
			if (strcmp(command, EXIT_COMMAND) == 0){
				getSpecificQueue(activeRemoteList, 
								 &remote_pid, 
								 &compareActiveRemotes, 
								 1
				);
				continue;

			} else{    //add the new active remote terminal to the list
				addQueue(&remote_pid, activeRemoteList);
			}

		} else{  // if it's a known terminal just copy the message
			strcpy(command, buf);
		}

		// get the command's arguments
		narg = getArguments(command, argVector, MAX_N_INPUT);

		// check for errors reading the input and read again if there were any
		if (narg == -1){
			fprintf(stderr, "Some error occurred getting the input from the fifo.\n");
			continue;
		}

		// case the command is a global exit
		if (strcmp(argVector[0], GLOBAL_EXIT_COMMAND) == 0){
			printf("Waiting for all the processes to terminate\n");
			break;
		}

		// case a terminal has exited remove it from the list

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
			xopen(filename, O_WRONLY | O_CREAT, READ_WRITE_ALL);
			
			// Change the process image to the program given by the user
			if (execv(argVector[0], argVector) < 0){ // check for errors
				fprintf(stderr,
						"Error occurred when trying to open the executable"
						" '%s': %s\n",
						argVector[0],
						strerror(errno));

				// case there was an error calling execv

				// free the allocated memory that was copied for the child process
				exitFree(argVector, processList, activeRemoteList, buf, 0);

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

	// TODO: close all remote terminals!!!!

	// indicate the thread to terminate
	par_shell_on = FALSE;

	// unlock monitor thread from waiting for child and exit
	xcond_signal(&numChildren_cond_variable);

	// terminate thread and destroy the locks
	exitThread(&thread_id, mutex_list, N_MUTEXES);

	xcond_destroy(&numChildren_cond_variable);

	// print final info and free allocated memory
	exitFree(argVector, processList, activeRemoteList, buf, 1);

	// close the fifo file, unlink the fifo and close the log file
	closeAll(parshell_fifo, PARSHELL_IN_FIFO, logFile);

	// exit the shell with success
	exit(EXIT_SUCCESS);
}
