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
// our own files
#include "commandsplitter.h"
#include "process_info.h"
#include "Auxiliares.h"
#include "Auxiliares-par-shell.h"

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

// filename format of the created processes output, its max size
#define OUTPUT_FILE_FORMAT "par-shell-out-%d.txt"
#define OUTPUT_NAME_MAX_SIZE 50
#define PERMISSIONS 0666

// the name of the FIFO for inputs
#define FIFO_NAME "par-shell-in"

//the format of the FIFO's for stats
#define STATS_FIFO_PATH_FORMAT "statsfifo-pid-%d"
#define MAX_FIFO_NAME_SIZE 30

// the messages from the terminals
#define START_MESSAGE "start"
#define EXIT_MESSAGE "exit"
#define EXIT_GLOBAL_MESSAGE "exit-global"
#define STATS_MESSAGE "stats"

int main(int argc, char* argv[]){

	// To initiate the par-shell no input arguments are needed
	if (argc != NARGS){
		printf("Usage: par-shell\n");
		exit(EXIT_FAILURE);
	}

	// initialize the SIGINT flag and associate the signal handler
	sigintFlag = FALSE;
	struct sigaction sa;
	sa.sa_handler = sigintHandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	xsigaction(SIGINT, &sa, NULL);

	// close the stdin and open the FIFO in its place
	xclose(0);
	xunlink(FIFO_NAME);
	xmkfifo(FIFO_NAME, FIFO_PERMISSIONS);
	xopen2(FIFO_NAME, O_RDONLY);

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

	// allocates the memory for the command that the terminals send
	char** argVector = (char**) xmalloc(sizeof(char*) * MAX_N_INPUT);
	argVector[0] = NULL;

	// initializes the list to store the children
	processList = initQueue();

	// Continue until the exit command is executed
	while (TRUE){

		Message message;
		int mret = readMessage(&message);

		// check for errors reading the message and read again if there were any
		if (mret == -1){
			fprintf(stderr, "Some error occurred reading a message from a terminal.\n");
			continue;
		}
		// in case read returned EOF
		// it means all terminals were terminated without calling exit
		if (mret == 1) {
			numTerminals = 0;
			// remove all terminals form the list
			while(!isEmptyQueue(terminalsList))
				free((pid_t *) getFirstQueue(terminalsList));
			// wait for a terminal to open the pipe
			waitFifo(FIFO_NAME, O_RDONLY);
			continue;
		}
		// case the SIGINT signal ocurred
		if ((mret == 2) && sigintFlag) {
			// kill all terminals and break the cicle
			killTerminals(terminalsList, 0);
			break;
		}

		// case its the start message
		if (message.type == START_M) {
			pid_t *pid = xmalloc(sizeof(pid_t));
			*pid = message.senderPid;
			addQueue(pid, terminalsList);
		  numTerminals++;
		  continue;
		}

		// case it is the exit message
		if (message.type == EXIT_M){
			// get the pid of the terminal that called exit and remove it from the list
			pid_t pid = message.senderPid;
			free((pid_t *) getSpecificQueue(terminalsList, &pid, comparePids, 1));
				if (--numTerminals == 0)
				// wait for a terminal to open the pipe
					waitFifo(FIFO_NAME, O_RDONLY);
			continue;
		}

		// case it is the exit-global message
		if (message.type == EXIT_GLOBAL_M) {
			pid_t callingPid = message.senderPid;
			// kill all processes in the list
			killTerminals(terminalsList, callingPid);
			break;
		}

		// case it is the stats message
		if (message.type == STATS_M) {
			// get the name of the fifo
			char fifoname[MAX_FIFO_NAME_SIZE];
			pid_t pid = message.senderPid;
			sprintf(fifoname, STATS_FIFO_PATH_FORMAT, pid);

			// open the fifo and free the path string
			int statsfifofd = xopen2(fifoname, O_WRONLY);

			// send the values, must lock the mutex
			mutex_lock(&numChildren_lock);
			xwrite(statsfifofd, &numChildren, sizeof(int));
			xwrite(statsfifofd, &execTime, sizeof(int));
			mutex_unlock(&numChildren_lock);

			xclose(statsfifofd);
			continue;
		}


		// case it was given a path to a program to execute
		if (message.type == COMMAND_M) {
			// wait if the limit of childs was reached
			mutex_lock(&numChildren_lock);
			while (numChildren >= MAXPAR)
				xcond_wait(&numChildren_cond_variable, &numChildren_lock);
			mutex_unlock(&numChildren_lock);

			// if the SIGINT signal ocurred while waiting
			if (sigintFlag){
				// kill all terminals and break the cicle
				killTerminals(terminalsList, 0);
				break;
			}

			pid_t child_pid = fork();    // create a new child process
			if (child_pid == -1){        // check for errors
				fprintf(stderr,
						"Error occurred when creating a new process: %s\n",
						strerror(errno));
				continue;
			}
			if (child_pid == 0){ 		 // child executes this

				// get the arguments in the message command
				readCommandArguments(argVector, MAX_N_INPUT, message.content);

				// close the stdout and open the output file in its place
				xclose(1);
				char filename[OUTPUT_NAME_MAX_SIZE];
				sprintf(filename, OUTPUT_FILE_FORMAT, getpid());
				xopen3(filename, O_WRONLY | O_CREAT, PERMISSIONS);

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
				continue;
			}
		}
	}
	/* exit-global command was given */

	// unlink the fifo
	xunlink(FIFO_NAME);

	printf("Waiting for all the processes to terminate\n");

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
