#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>

#include <pthread.h>
#include <process_info.h>

#include "QUEUE.h"
#include "Auxiliares.h"
#include "parseInput.h"

// number of input arguments
#define N_ARGS 2

int main(int argc, char const *argv[])
{
	if (argc != N_ARGS){
		fprintf(stdout, "Usage: par-shell-terminal pathname\n");
		exit(EXIT_FAILURE);
	}

	// open the fifo to write
	int f_write = xopen(argv[1], O_WRONLY, READ_WRITE_EXEC_ALL);

	// str to store the input
	char buf[BUFSIZ + 1];
	char str_to_send[SZ];

	// stores the output of the getLine function
	int out;

	//indicates if it is the first command this terminal is sending
	int first_command = TRUE;

	while (TRUE){

		// clean string to send
		strcpy(str_to_send, "");

		// get the command
		out = getLine(buf, BUFSIZ);

		// check for errors reading the input and read again if there were any
		if (out == -1){
			fprintf(stderr, "Some error occurred reading the user's input.\n");
			continue;
		
		} else if (out == 2){ // in case no command was inserted 
			fprintf(stdout, "Please input a valid command\n");
			continue;
		}

		// add the pid to the string to send if it is the first message
		if (first_command || strcmp(buf, EXIT_COMMAND) == 0){
			pid_t my_pid = getpid();
			sprintf(str_to_send, "\a%d ", my_pid);
			first_command = FALSE;
		}

		//update str_to_send with the new command
		strcat(str_to_send, buf);

		// write to the fifo
		xwrite(f_write, str_to_send, BUFSIZ);

		// if the command sent was to exit, exit. Still send the command to the fifo
		// to avoid erros 
		if (strcmp(buf, EXIT_COMMAND) == 0) break;

	}

	xclose(f_write);

	return 0;
}