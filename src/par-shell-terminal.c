#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>

#include "Auxiliares.h"
#include "Auxiliares-terminal.h"

#define EXIT_COMMAND "exit\n"
#define EXIT_GLOBAL_COMMAND "exit-global\n"
#define STATS_COMMAND "stats\n"

#define START_MESSAGE "\a start %d"
#define EXIT_MESSAGE "\a exit %d"
#define EXIT_GLOBAL_MESSAGE "\a exit-global %d"
#define STATS_MESSAGE "\a stats %d"

#define FIFO_PATH_FORMAT "%s/statsfifo-pid-%d"
#define MAX_FIFO_NAME_SIZE 25

// the number of arguments of the par-shell-terminal
#define NARGS 2

int main(int argc, char const *argv[]) {

  // To initiate the par-shell-terminal the name of the pipe for output is needed
  if (argc != NARGS) {
    printf("Usage: %s <par-shel pipe>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // open the par-shell pipe
  int parshellFd = xopen2(argv[1], O_WRONLY);

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
  if (strcmp(line, EXIT_COMMAND) == 0) {
    sendMessage(EXIT_MESSAGE, parshellFd);
    break;
  }

  // the exit-global command
  if (strcmp(line, EXIT_GLOBAL_COMMAND) == 0) {
    sendMessage(EXIT_GLOBAL_MESSAGE, parshellFd);
    break;
  }

  // the stats command
  if (strcmp(line, STATS_COMMAND) == 0) {
    // copy the content of argv1 to an aux string
    char *argvCopy = malloc(strlen(argv[1]));
    strcpy(argvCopy, argv[1]);

    // create the input fifo in the same directory as the output fifo
    char *fifopath = malloc(strlen(argvCopy) + MAX_FIFO_NAME_SIZE);
    sprintf(fifopath, FIFO_PATH_FORMAT, dirname(argvCopy), getpid());
    free(argvCopy);
    xmkfifo(fifopath, FIFO_PERMISSIONS);

    // send the message to par-shell
    sendMessage(STATS_MESSAGE, parshellFd);

    // open the fifo
    int fifofd = xopen2(fifopath, O_RDONLY);

    // the numbers to get from par-shell
    int numProcesses, totalTime;
    read(fifofd, &numProcesses, sizeof(int)); //TODO change to xread
    read(fifofd, &totalTime, sizeof(int));
    // close the fifo
    xclose(fifofd);

    printf("Number of active processes: %d\n", numProcesses);
    printf("Total execution time: %d\n", totalTime);
    continue;
  }

  // if the input was not a command send it to par-shell
  xwrite(parshellFd, line, s);
}

  free(line);
  xclose(parshellFd);
  return 0;
}
