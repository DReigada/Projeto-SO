#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include "Auxiliares-terminal.h"
#include "Auxiliares.h"

/**
 * Sends a message (seting its type first) to the pipe with file descriptor fd
 */
void sendMessage(Message *message, int type, int fd){
  message->type = type;
  xwrite(fd, message, sizeof(Message));
}

/**
 * The handler for SIGTERM
 */
void sigtermHandler(int signal){
  sigtermFlag = TRUE;
}
