#ifndef _PROC_INFO_H_
#define _PROC_INFO_H_

#include <time.h>
#include <sys/types.h>



/* Struct with the information about a process */
typedef struct process_info_s {
   pid_t pid;
   int exitStatus;
   time_t startTime;
   time_t endTime;
}* process_info;

/**
 * Initialize the process.
 * Takes as inputs the process pid and its start time.
 * Returns the new process_info.
 */
process_info createProcessInfo(int pid, time_t startTime);

/**
 * Get the PID of a process.
 * Takes the process_info as input.
 * Returns its PID. 
 */
#define getPid(P) ((P) -> pid)

/**
 * Get the star time of a process.
 * Takes the process_info as input.
 * Returns its start time. 
 */
#define getStartTime(P) ((P) -> startTime)

/**
 * Get the end time of a process.
 * Takes the process_info as input.
 * Returns its end time (-1 if it was not set). 
 */
#define getEndTime(P) ((P) -> endTime)

/**
 * Get the exit status of a process.
 * Takes the process_info as input.
 * Returns its return exit status as an int (-1 if it was not set). 
 */
#define getExitStatus(P) ((P) -> exitStatus)

/**
 * Set the end time of a process.
 * Takes as input the process_info and the end time.
 * Returns nothing.
*/
#define setEndTime(P, T) ((P) -> endTime = (T))

/**
 * Set the exit status of a process.
 * Takes as input the process_info and the status as an int.
 * Returns nothing.
*/
#define setExitStatus(P, S) ((P) -> exitStatus = (S))

/** 
 * Macro to free a process info.
 * It takes as input the process.
 */
#define freeProcInfo(P) (free(P));


#endif