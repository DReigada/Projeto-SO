/* Projeto de SO */

#include <stdio.h>
#include <stdlib.h>
#include "commandlinereader.h"
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "QUEUE.h"
#include "process_info.h"

#define MAX_N_INPUT 7 // the programs executed in the par-shell are limited to 
					  // 5 input arguments (the last entry is always set to NULL)

int main(int argc, char* argv[]){

	// To initiate the par-shell no input arguments are needed
	if (argc != 1){
		printf("Usage: par-shell\n");
		exit(EXIT_FAILURE);
	}

	// allocates the memory for the command that the user inputs
	char** argVector = (char**) malloc(sizeof(char*) * 7); 			//CHANGE to xmalloc
	if (argVector == NULL){
		fprintf(stderr, "Error allocating argVector's memory. %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	// stores the number of arguments from the user
	int narg = 0;

	// initialize the list to store the processes
	Queue processList = initQueue();

	// Continue until the exit command is executed
	while (1){
		
		// read the user input
		narg = readLineArguments(argVector, MAX_N_INPUT);

		// checks for errors reading the input and reads again if there were
		if (narg == -1){
			fprintf(stderr, "Some error occurred reading the user's input.\n");
			continue;
		}

		/* see what command the user typed */

		// exit
		if (strcmp(argVector[0], "exit") == 0)
			// TODO: precisa de ser alterada esta parte de acordo com as especificações!!!
			exit(EXIT_SUCCESS);

		// else we assume it was given a path to a program to execute
		pid_t child_pid = fork();

		// check for errors in the creation of a new process
		if (child_pid == -1){
			fprintf(stderr, "Some error occurred when creating a new process. %s\n", strerror(errno));
			continue;
		}

		// child executes this
		if (child_pid == 0){
			
			// Change the process image to the program given by the user
			int err = execv(argVector[0], argVector);

			// Check for errors
			if (err == -1){
				fprintf(stderr, "Erro ao tentar abrir programa com o pathname. %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}

			// exit with success if there are no errors
			else
				exit(EXIT_SUCCESS);
		}

		// parent executes this
		else{
			process_info process = createProcessInfo(child_pid, time(NULL));
			addQueue(process, processList);
		}
	}
}








