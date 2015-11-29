#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "Auxiliares.h"

#define EXIT_COMMAND "exit"

#define MESSAGE_MAX_SIZE 50
#define EXIT_MESSAGE "\a exit %d"

// the number of arguments of the par-shell-terminal
#define NARGS 2


int main(int argc, char const *argv[]) {

  // To initiate the par-shell-terminal the name of the pipe for output is needed
  if (argc != NARGS) {
    printf("Usage: %s <par-shel pipe>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // close the stdout and redirect the outputs to the par-shell pipe
  int std = dup(STDOUT_FILENO);
  xclose(STDOUT_FILENO);
  xopen(argv[1], O_WRONLY, 0666); //TODO 2 arguments

  char *line = NULL;
  size_t size = 0;

while (1) {
  int s = getline(&line, &size, stdin); // get the input from the user

  // the exit command
  if (strncmp(line, EXIT_COMMAND, s - 1) == 0) {
    char message[MESSAGE_MAX_SIZE];
    sprintf(message, EXIT_MESSAGE, getpid());
    break;
  }

  // if the input was not a command send it to par-shell
  else{
    xwrite(STDOUT_FILENO, line, s);
  }

}

  free(line);
  return 0;
}
