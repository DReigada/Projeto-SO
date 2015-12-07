/*File that defines a message to be sent from par-shell-terminal to par-shell*/

#ifndef _MESSAGE_H_
#define _MESSAGE_H_


#include <sys/types.h>

#define MAX_CONTENT_SIZE 100

// message type definitions
#define START_M 1
#define EXIT_M 2
#define EXIT_GLOBAL_M 3
#define STATS_M 4
#define COMMAND_M 5

typedef struct{
  int type;
  pid_t senderPid;
  char content[MAX_CONTENT_SIZE];
} Message;

#endif
