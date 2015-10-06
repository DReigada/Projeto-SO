/*File that defines two functions for a better use of malloc and realloc*/
#ifndef _XMALLOC_H_
#define _XMALLOC_H_


/**
 * Uses the same input as malloc, and has the same output, with the only 
 * difference being that it stops execution if some error occurred when
 * calling malloc.
 */
void* xmalloc(unsigned siz);


/**
 * Uses the same input as realloc, and has the same output, with the only 
 * difference being that it stops execution if some error occurred when
 * calling realloc.
 */
void* xrealloc(void* ptr, unsigned siz);


#endif
