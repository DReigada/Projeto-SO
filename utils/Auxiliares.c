/*File that defines global auxiliary functions*/
#include "Auxiliares.h"

#include <string.h>
#include <errno.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/**
 * Uses the same input as malloc, and has the same output, with the only
 * difference being that it stops execution if some error occurred when
 * calling malloc.
 */
void* xmalloc (unsigned siz){
    void* mem = malloc(siz); /* allocate the needed memory */

	if (mem == NULL){    /* check for errors in allocating memory */
		fprintf(stderr, "No memory in line %d of file \"%s\".\n",
   		__LINE__, __FILE__); exit(2);}

	return mem;
}

/**
 * Uses the same input as fopen but if some error occurs when calling fopen
 * it does not return null, instead it stops the execution.
 */
FILE *xfopen(const char *path, const char *mode){
   FILE *file;
   if((file = fopen(path, mode)) == NULL){
     fprintf(stderr, "Error opening/creating log file: %s\n", strerror(errno));
     exit(EXIT_FAILURE);
   }
   return file;
 }

/**
 * Uses the same input as freopen but if some error occurs when calling freopen
 * it does not return null, instead it stops the execution.
 */
FILE *xfreopen(const char *path, const char *mode, FILE *stream){
   if((stream = freopen(path, mode, stream)) == NULL){
     fprintf(stderr, "Error reopening log file: %s\n", strerror(errno));
     exit(EXIT_FAILURE);
   }
   return stream;
 }

 /**
  * Uses the same input as fclose (and no output), with the only
  * difference being that it stops execution if some error occurred when
  * calling fclose.
  */
void xfclose(FILE *fp){
   if(fclose(fp) != 0){
     fprintf(stderr, "Error closing log file: %s\n", strerror(errno));
     exit(EXIT_FAILURE);
   }
 }

 /**
  * Uses the same input as fflush (and no output), with the only
  * difference being that it stops execution if some error occurred when
  * calling flush.
  */
void xfflush(FILE *stream){
  if (fflush(stream) != 0) {
    fprintf(stderr, "Error flushing log file: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
}

/**
 * Uses the same input as open but if some error occurs when calling open
 * it does not return null, instead it stops the execution.
 * Returns the file descriptor in success.
 */
int xopen(const char *pathname, int flags, mode_t mode){
  int fd;
  if((fd = open(pathname, flags, mode)) == -1){
    fprintf(stderr, "Error opening file: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  return fd;
}

/**
 * Uses the same input as close (and no output), with the only
 * difference being that it stops execution if some error occurred when
 * calling close.
 */
void xclose(int fd){
  if(close(fd) == -1){
    fprintf(stderr, "Error closing file: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

}

/**
 * Uses the same input as write but if some error occurs when calling write
 * it does not return -1, instead it stops the execution.
 * Returns the  number  of bytes written.
 */
ssize_t xwrite(int fd, const void *buf, size_t count){
  if (write(fd, buf, count) == -1) {
    fprintf(stderr, "Error writing to file/pipe %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
}
