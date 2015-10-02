/* Projeto de SO */

#include <stdio.h>
#include <stdlib.h>
#include "commandlinereader.h"
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#define MAX_N_INPUT 7 // the programs executed in the par-shell are limited to 
					  // 5 input arguments (the last entry is always set to NULL)

int main(int argc, char* argv[]){

	// To initiate the par-shell no input arguments are needed
	if (argc != 1){
		printf("Usage: par-shell\n");
		exit(EXIT_FAILURE);
	}

	// allocates the memory for the command that the user inputs
	char** argVector = (char**) malloc(sizeof(char*) * 7);
	if (argVector == NULL){
		fprintf(stderr, "Error allocating argVector's memory\n");
		exit(EXIT_FAILURE);
	}

	// stores the number of arguments from the user
	int narg = 0;

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
			fprintf(stderr, "Some error occurred when creating a new process.\n");
			continue;
		}

		// child executes this
		if (child_pid == 0){
			/* TODO: executar o programa do ficheiro introduzido */
		}

		
		
	}
}








