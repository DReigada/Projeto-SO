/* File that implements a QUEUE (FIFO method) */
#include "QUEUE.h"

/** 
 * Macro to free a node of the queue.
 * This depends on the type of item you chose, so it should be modified 
 * as needed.
 * It takes as input the pointer to the node being freed (type QLink).
 */
 #define freeNodeQ(A) (free(A))


/**
 * Initializes the queue.
 * Takes in no arguments and returns the pointer to the head, 
 * which we just call Head. 
 * Returns the pointer to the new node.
 */
 Queue initQueue(){
 	Queue q = (Queue) xmalloc(sizeof(struct queue)); /* allocate memory for queue */
 	q->head = NULL; /* initializing it's head and tail to NULL */
 	q->tail = NULL;
 	return q;
 }

/**
 * Initializing a new node.
 * Takes in as input the node item and the pointer to the next node
 * Returns the pointer to the new node.
 */
 QLink newNode(void* i, QLink nextNode){
 	QLink n = (QLink) xmalloc(sizeof(struct nodeQ)); /* allocate memory for the node */

 	n->item = i; /* assigning the new node it's item */
 	n->next = nextNode; /* giving it the pointer to the next node */

 	return n;
 }


/**
 * Adding a new element to the queue.
 * This element will be added in the end of the queue as it's tail
 * The inputs are the element's item and the queue which to add the element
 * Nothing is returned.
 */
 void addQueue(void* i, Queue q){
 	QLink node = newNode(i, NULL); /* creating a new node pointing to NULL */

 	/* if we are adding a new element to an empty queue, the head and the tail
 	   will be the same and equal to this element, which is the only one */
 	if (isEmptyQueue(q)){ 
 		q->tail = node;
 		q->head = q->tail;
 		return;
 	}

 	q->tail->next = node; /* my previous tail now points to the new element */
 	q->tail = node; /* the new queue's tail is the new node */
}


/**
 * Retrieves the element that has been in the queue the longest - the head - 
 * (FIFO method) and so actualizes queue's head.
 * Takes as input the queue.
 * Returns the retrieved element's item if the queue has any elements.
 */
void* getFirstQueue(Queue q){
	void* it;
	QLink temp;
	if (isEmptyQueue(q)) return NULL; /* if empty, no element to retrieve */

	it = q->head->item;
	temp = q->head->next; /* saving the pointer so we don't lose it */

	freeNodeQ(q->head); /* freeing the memory allocated for the node */
	q->head = temp; /* setting the queue's new head */
	return it;
}

