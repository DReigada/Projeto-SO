/*File that defines auxiliary functions used in par-shell-terminal*/
#ifndef _AUXILIARES_TERMINAL_H_
#define _AUXILIARES_TERMINAL_H_

#define MESSAGE_MAX_SIZE 50

/**
 * Sends a message with msgFormat to the file/pipe with file descriptor fd
 */
void sendMessage(char *msgFormat, int fd);


#endif
