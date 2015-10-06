#include "process_info.h"
#include "Xmalloc.h"
#include <stdlib.h>

/**
 * Initialize the process.
 * Takes as inputs the process pid and its start time.
 * Returns the new process_info.
 */
process_info createProcessInfo(int pid, time_t startTime){
	process_info process = (process_info) xmalloc(sizeof(struct process_info_s));
	process -> pid = pid;
	process -> startTime = startTime;
	process -> endTime = -1;			//initialize the end time with -1 to know if it was set or not
	return process;
}