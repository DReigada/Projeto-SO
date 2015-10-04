
#ifndef _PROC_INFO_H_
#define _PROC_INFO_H_

#include <stdlib.h>
#include "Xmalloc.h"


/* Struct with the information about a process */
typedef struct process_info_s {
   int pid;
   time_t starttime;
   time_t endtime;
}* process_info;

/**
 * Initialize the item.
 * Takes as inputs the process pid and its start time.
 * Returns the new process_info.
 */
process_info createItem(int pid, time_t starttime);

/**
 * Get the PID of a process.
 * Takes the process_info as input.
 * Returns its PID. 
 */
int getPid(process_info item);

/**
 * Get the star time of a process.
 * Takes the process_info as input.
 * Returns its start time. 
 */
time_t getStartTime(process_info item);

/**
 * Get the end time of a process.
 * Takes the process_info as input.
 * Returns its end time (-1 if its end time was not set). 
 */
time_t getEndTime(process_info item);


/**
 * Set the end time of a process.
 * Takes as input the process_info and the end time.
 * Returns nothing.
*/
void setEndTime(process_info item, time_t endtime);


#endif