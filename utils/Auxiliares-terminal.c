#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include "Auxiliares-terminal.h"
#include "Auxiliares.h"

/**
 * Sends a message with msgFormat to the file/pipe with file descriptor fd
 */
void sendMessage(char *msgFormat, int fd){
  char message[MESSAGE_MAX_SIZE];
  int size = sprintf(message, msgFormat, getpid());
  xwrite(fd, message, size);
}
