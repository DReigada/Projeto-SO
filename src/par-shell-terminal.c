#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "Auxiliares.h"
#include "Auxiliares-terminal.h"

#define EXIT_COMMAND "exit"
#define EXIT_GLOBAL_COMMAND "exit-global"

#define START_MESSAGE "\a start %d"
#define EXIT_MESSAGE "\a exit %d"
#define EXIT_GLOBAL_MESSAGE "\a exit-global %d"

// the number of arguments of the par-shell-terminal
#define NARGS 2

int main(int argc, char const *argv[]) {

  // To initiate the par-shell-terminal the name of the pipe for output is needed
  if (argc != NARGS) {
    printf("Usage: %s <par-shel pipe>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // open the par-shell pipe
  int parshellFd = xopen(argv[1], O_WRONLY, 0666); //TODO 2 arguments

  // send the start message
  sendMessage(START_MESSAGE, parshellFd);

  char *line = NULL;
  size_t size = 0;

while (1) {
  // get the input from the user
  int s = getline(&line, &size, stdin);

  // case it returned an error
  if (s == -1) {
    fprintf(stderr, "Some error occurred reading the user's input. %s\n", strerror(errno));
    continue;
  }
 // case it read an invalid command
  if (s < 2) {
    fprintf(stdout, "Please input a valid command\n");
    continue;
  }
  // the exit command
  if (strncmp(line, EXIT_COMMAND, MAX(strlen(EXIT_COMMAND), strlen(line) - 1)) == 0) {
    sendMessage(EXIT_MESSAGE, parshellFd);
    break;
  }

  // the exit-global command
  if (strncmp(line, EXIT_GLOBAL_COMMAND, MAX(strlen(EXIT_GLOBAL_COMMAND), strlen(line) -1)) == 0) {
    sendMessage(EXIT_GLOBAL_MESSAGE, parshellFd);
    break;
  }

  // if the input was not a command send it to par-shell
  xwrite(parshellFd, line, s);
}

  free(line);
  xclose(parshellFd);
  return 0;
}
