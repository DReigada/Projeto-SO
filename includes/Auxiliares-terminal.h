/*File that defines auxiliary functions used in par-shell-terminal*/
#ifndef _AUXILIARES_TERMINAL_H_
#define _AUXILIARES_TERMINAL_H_

#include "Message.h"

#define MESSAGE_MAX_SIZE 50

#ifdef DEFINE_VARIABLES
#define EXTERN /* nothing */
#else
#define EXTERN extern
#endif

// the flag for the SIGINT handler
EXTERN int sigtermFlag;

/**
 * Sends a message (seting its type first) to the pipe with file descriptor fd
 */
void sendMessage(Message *message, int type, int fd);

/**
 * The handler for SIGTERM
 */
void sigtermHandler(int signal);

#endif
