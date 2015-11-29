#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "Auxiliares.h"


// the number of arguments of the par-shell-terminal
#define NARGS 2


int main(int argc, char const *argv[]) {

  // To initiate the par-shell-terminal the name of the pipe for output is needed
  if (argc != NARGS) {
    printf("Usage: %s <par-shel pipe>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // close the stdout and redirect the outputs to the par-shell pipe
  xclose(STDOUT_FILENO);
  xopen(argv[1], O_WRONLY, 0666); //TODO 2 arguments

  char *line = NULL;
  size_t size = 0;

while (1) {
  int i = getline(&line, &size, stdin);
  write(STDOUT_FILENO, line, i); // TODO xwrite
}


  free(line);
  return 0;
}
