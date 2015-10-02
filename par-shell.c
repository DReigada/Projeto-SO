/* Projeto de SO */

#include <stdio.h>
#include <stdlib.h>
#include "commandlinereader.h"
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char* argv[]){

	// To initiate the par-shell no input arguments are needed
	if (argc =! 1){
		printf("Usage: par-shell\n");
		exit(EXIT_FAILURE);
	}

	
}








