#include <fcntl.h>
#include <unistd.h>

#include "Auxiliares.h"

// number of input arguments
#define N_ARGS 2

int main(int argc, char const *argv[])
{
	if (argc != N_ARGS){
		fprintf(stdout, "Usage: par-shell-terminal pathname\n");
		exit(EXIT_FAILURE);
	}

	// open the fifo to write
	int f_write = xopen(PARSHELL_IN, O_WRONLY, READ_WRITE_EXEC_ALL);

	xclose(f_write);

	return 0;
}