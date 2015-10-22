/* Defines the function that the monitor thread for the par-shell is going to run */
#ifndef _THREADFUNCTION_H
#define _THREADFUNCTION_H

/**
 * Monitors the active children processes. Waits for them to finish and
 * when they have it terminates them and sets their end time
 * 
 * @param arg is it's sole argument
 * Returns 0 if exited with no errors and -1 otherwise
 */
 void* monitorChildProcesses();

#endif


