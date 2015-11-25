#include <stdlib.h>
#include <stdio.h>

#include <fcntl.h>
#include <unistd.h>

#include <pthread.h>
#include <process_info.h>

#include "QUEUE.h"
#include "Auxiliares.h"
#include "commandlinereader.h"

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

	while (TRUE){

		// get the command
		getLine(str, BUFSIZ);

		//TODO: testar se vem o comando exit

		// escrever para o fifo
		xwrite(f_write, str, BUFSIZ);


	}
	xclose(f_write);

	return 0;
}