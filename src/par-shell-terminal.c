#include <stdlib.h>
#include <stdio.h>

#include <fcntl.h>
#include <unistd.h>

#include <pthread.h>
#include <process_info.h>

#include "QUEUE.h"
#include "Auxiliares.h"
#include "parseInput.h"

// number of input arguments
#define N_ARGS 2

#define SZ BUFSIZ

int main(int argc, char const *argv[])
{
	if (argc != N_ARGS){
		fprintf(stdout, "Usage: par-shell-terminal pathname\n");
		exit(EXIT_FAILURE);
	}

	// open the fifo to write
	int f_write = xopen(PARSHELL_IN_FIFO, O_WRONLY, READ_WRITE_EXEC_ALL);

	// str to store the input
	char str[BUFSIZ - 1];

	// stores the output of the getLine function
	int out;

	while (TRUE){

		// get the command
		out = getLine(str, BUFSIZ);

		// check for errors reading the input and read again if there were any
		if (out == -1){
			fprintf(stderr, "Some error occurred reading the user's input.\n");
			continue;
		}
		else if (out == 0){ // in case no command was inserted
			fprintf(stdout, "Please input a valid command\n");
			continue;
		}
		//TODO: testar se vem o comando exit

		// escrever para o fifo
		xwrite(f_write, str, BUFSIZ);


	}
	xclose(f_write);

	return 0;
}