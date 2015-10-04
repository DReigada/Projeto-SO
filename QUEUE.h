/* File that implements a QUEUE (FIFO method) */
#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdlib.h>
#include "Xmalloc.h"



/*Structures*/

/** 
 * Each node of the queue contains the pointer to the actual item we want to
 * and the pointer to the next node 
 */
typedef struct nodeQ{
	void* item;
	struct nodeQ* next;	
}* QLink;


/**
 * The queue will be only defined by it's head and it's tail. When we add
 * new nodes they enter as being the new tail and the most efficient way to
 * take elements is by the head (FIFO), although you can also specify a element
 * to take using a different method.
 */
 typedef struct queue{
 	QLink head;
 	QLink tail;
 }* Queue;



/*Functions*/

/**
 * Initializes the queue.
 * Takes in no arguments and returns the pointer to the head, 
 * which we just call Head. 
 * Returns the pointer to the new node.
 */
 Queue initQueue();


/** 
 * Macro to free a queue.
 * It takes as input the Queue.
 */

#define freeQ(A) (free(A))


/**
 * Adds a new element to the queue.
 * This element will be added in the end of the queue as it's tail
 * The inputs are the element's item and the queue which to add the element
 * Returns the updated queue.
 */
 void addQueue(void* i, Queue q);


/**
 * Macro to check to see if the queue is empty.
 * Takes as input a queue and returns 1 if empty and 0 otherwise.
 * Using a macro to define this function.
 */
 #define isEmptyQueue(A) ((A)->head == NULL)

/**
 * Retrieves the element that has been in the queue the longest - the head - 
 * (FIFO method) and so actualizes the queue's head.
 * Takes as input the queue.
 * Returns the retrieved element's item if the queue has any elements and
 * NULL if not.
 */
 void* getFirstQueue(Queue q);


#endif
