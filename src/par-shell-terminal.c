#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>

#define DEFINE_VARIABLES
#include "Auxiliares.h"
#include "Auxiliares-terminal.h"

#include "Message.h"

// the special commands to compare with
#define EXIT_COMMAND "exit\n"
#define EXIT_GLOBAL_COMMAND "exit-global\n"
#define STATS_COMMAND "stats\n"

#define FIFO_PATH_FORMAT "%s/statsfifo-pid-%d"
#define MAX_FIFO_NAME_SIZE 30

// the number of arguments of the par-shell-terminal
#define NARGS 2

int main(int argc, char const *argv[]) {

  // To initiate the par-shell-terminal the name of the pipe for output is needed
  if (argc != NARGS) {
    printf("Usage: %s <par-shel pipe>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // initialize the SIGTERM flag and associate the signal handler
  sigtermFlag = FALSE;
  struct sigaction sa;
  sa.sa_handler = sigtermHandler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  xsigaction(SIGTERM, &sa, NULL);

  // open the par-shell pipe
  int parshellFd = xopen2(argv[1], O_WRONLY);

  // send the start message
  Message startMessage;
  startMessage.senderPid = getpid();
  sendMessage(&startMessage, START_M, parshellFd);

  char *line = NULL;
  size_t size = 0;

while (1) {

  // get the input from the user
  int s = getline(&line, &size, stdin);

  // case it received the sigterm interrupt
  if (sigtermFlag == TRUE) {
    printf("Terminal was forced to shutdown\n");
    break;
  }
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

  // create the message and set the pid
  Message message;
  message.senderPid = getpid();

  // the exit command
  if (strcmp(line, EXIT_COMMAND) == 0) {
    sendMessage(&message, EXIT_M, parshellFd);
    break;
  }

  // the exit-global command
  if (strcmp(line, EXIT_GLOBAL_COMMAND) == 0) {
    sendMessage(&message, EXIT_GLOBAL_M, parshellFd);
    break;
  }

  // the stats command
  if (strcmp(line, STATS_COMMAND) == 0) {
    // copy the content of argv1 to an aux string
    char *argvCopy = malloc(strlen(argv[1]) + 1);
    strcpy(argvCopy, argv[1]);

    // create the input fifo in the same directory as the output fifo
    char *fifopath = malloc(strlen(argvCopy) + MAX_FIFO_NAME_SIZE);
    sprintf(fifopath, FIFO_PATH_FORMAT, dirname(argvCopy), getpid());
    free(argvCopy);
    xunlink(fifopath);
    xmkfifo(fifopath, FIFO_PERMISSIONS);

    // send the message to par-shell
    sendMessage(&message, STATS_M, parshellFd);

    // open the fifo
    int fifofd = xopen2(fifopath, O_RDONLY);

    // the numbers to get from par-shell
    int numProcesses, totalTime;
    xread(fifofd, &numProcesses, sizeof(int));
    xread(fifofd, &totalTime, sizeof(int));
    // close and delete the fifo
    xclose(fifofd);
    xunlink(fifopath);
    // free the path string
    free(fifopath);

    printf("Number of active processes: %d\n", numProcesses);
    printf("Total execution time: %d\n", totalTime);
    continue;
  }

  // if the input was not a special command send it to par-shell
  // unless the command was too long
  if (s - 1 > MAX_CONTENT_SIZE) {
    printf("Input is too long\n");
    continue;
  }
  strcpy(message.content, line);
  sendMessage(&message, COMMAND_M, parshellFd);
}

  free(line);
  xclose(parshellFd);
  return 0;
}
