/*File that defines two functions for a better use of malloc and realloc*/
#include "Xmalloc.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * Uses the same input as malloc, and has the same output, with the only 
 * difference being that it stops execution if some error occurred when
 * calling malloc.
 */
  void* xmalloc(unsigned siz){
    void* mem = malloc(siz); /* allocate the needed memory */

	 if (mem == NULL){ fprintf(stderr, "No memory in line %d of file \"%s\".\n",
	   	 __LINE__, __FILE__); exit(2);} /* check for errors in allocating memory */

		  return mem;
  }


/**
 * Uses the same input as realloc, and has the same output, with the only 
 * difference being that it stops execution if some error occurred when
 * calling realloc.
 */
void* xrealloc(void* ptr, unsigned siz){
    void* mem = realloc(ptr, siz); /* allocate the needed memory */

	 if (mem == NULL){ fprintf(stderr, "No memory in line %d of file \"%s\".\n",
	   	 __LINE__, __FILE__); exit(2);} /* check for errors in allocating memory */

		  return mem;
  }
