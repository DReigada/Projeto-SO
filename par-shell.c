/* Projeto de SO */

#include <stdio.h>
#include <stdlib.h>
#include "commandlinereader.h"
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>

#define MAX_N_INPUT 7 // the programs executed in the par-shell are limited to 
					  // 5 input arguments (the last entry is always set to NULL)

int main(int argc, char* argv[]){

	// To initiate the par-shell no input arguments are needed
	if (argc =! 1){
		printf("Usage: par-shell\n");
		exit(EXIT_FAILURE);
	}

	// allocates the memory for the command that the user inputs
	char** argVector = (char**) malloc(sizeof(char*) * 7);
	if (argVector == NULL){
		frpintf(stderr, "Error allocating argVector's memory");
		exit(EXIT_FAILURE);
	}

	// Continue until the exit command is executed
	while(1){
		

		
	}
}








