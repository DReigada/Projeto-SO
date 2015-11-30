/*File that defines global auxiliary functions*/
#ifndef _AUXILIARES_H_
#define _AUXILIARES_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

// boolean values
#define TRUE 1
#define FALSE 0

/**
 * Returns the larger of 2 numbers
 */
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))


/**
 * Uses the same input as malloc, and has the same output, with the only
 * difference being that it stops execution if some error occurred when
 * calling malloc.
 */
void* xmalloc(unsigned siz);

/**
 * Uses the same input as fopen but if some error occurs when calling fopen
 * it does not return null, instead it stops the execution.
 */
FILE *xfopen(const char *path, const char *mode);

/**
 * Uses the same input as freopen but if some error occurs when calling freopen
 * it does not return null, instead it stops the execution.
 */
FILE *xfreopen(const char *path, const char *mode, FILE *stream);

 /**
  * Uses the same input as fclose (and no output), with the only
  * difference being that it stops execution if some error occurred when
  * calling fclose.
  */
void xfclose(FILE *fp);

/**
 * Uses the same input as fflush (and no output), with the only
 * difference being that it stops execution if some error occurred when
 * calling flush.
 */
void xfflush(FILE *stream);

/**
 * Uses the same input as open but if some error occurs when calling open
 * it does not return null, instead it stops the execution.
 */
int xopen(const char *pathname, int flags, mode_t mode);

/**
 * Uses the same input as close (and no output), with the only
 * difference being that it stops execution if some error occurred when
 * calling close.
 */
void xclose(int fd);

/**
 * Uses the same input as write but if some error occurs when calling write
 * it does not return -1, instead it stops the execution.
 * Returns the  number  of bytes written.
 */
ssize_t xwrite(int fd, const void *buf, size_t count);

/**
 * Uses the same input as unlink (and no output), with the only
 * difference being that it stops execution if some error occurred when
 * calling unlink.
 */
void xunlink(const char *pathname);

#endif
