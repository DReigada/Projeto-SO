/*File that defines auxiliary functions used in par-shell*/
#include "Auxiliares-par-shell.h"
#include "Auxiliares.h"
#include "process_info.h"

#include <string.h>
#include <errno.h>
#include <signal.h>

#define MAXLINESIZE 50

#define ITERATION_FORMAT "iteracao %d\n"
#define PID_FORMAT "pid: %d execution time: %d s\n"
#define EXECTIME_FORMAT "total execution time: %d s\n"

#define TOKEN_DELIMITER " \n\r\t"

/**
 * Uses the same input as pthread_mutex_lock (and no output), with the only
 * difference being that it stops execution if some error occurred when
 * calling pthread_mutex_lock.
 */
void mutex_lock(pthread_mutex_t* lock){
	int err;

	if ((err = pthread_mutex_lock(lock)) != 0){
		fprintf(stderr, "Error locking the mutex: %s\n", strerror(err));
		exit(EXIT_FAILURE);
	}
}

/**
 * Uses the same input as pthread_mutex_unlock (and no output), with the only
 * difference being that it stops execution if some error occurred when
 * calling pthread_mutex_unlock.
 */
void mutex_unlock(pthread_mutex_t* lock){
	int err;

	if ((err = pthread_mutex_unlock(lock)) != 0){
		fprintf(stderr, "Error unlocking the mutex: %s\n", strerror(err));
		exit(EXIT_FAILURE);
	}
}

/**
 * Uses the same input as pthread_cond_init (and no output), with the only
 * difference being that it stops execution if some error occurred when
 * calling pthread_cond_init.
 */
void xcond_init(pthread_cond_t *cond, const pthread_condattr_t *attr){

  if (pthread_cond_init(cond, attr) != 0){
    fprintf(stderr, "Error initializing the condition variable.\n");
    exit(EXIT_FAILURE);
  }
}

/**
 * Uses the same input as pthread_cond_destroy (and no output), with the only
 * difference being that it stops execution if some error occurred when
 * calling pthread_cond_destroy.
 */
void xcond_destroy(pthread_cond_t *cond){

  if (pthread_cond_destroy(cond) != 0){
    fprintf(stderr, "Error destroying the condition variable.\n");
    exit(EXIT_FAILURE);
  }
}

/**
 * Uses the same input as pthread_cond_wait (and no output), with the only
 * difference being that it stops execution if some error occurred when
 * calling pthread_cond_wait.
 */
void xcond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex){

  if (pthread_cond_wait(cond, mutex) != 0){
    fprintf(stderr, "Error waiting for the condition variable");
    exit(EXIT_FAILURE);
  }
}

/**
 * Uses the same input as pthread_cond_signal (and no output), with the only
 * difference being that it stops execution if some error occurred when
 * calling pthread_cond_signal.
 */
void xcond_signal(pthread_cond_t *cond){

  if (pthread_cond_signal(cond) != 0){
    fprintf(stderr, "Error unlocking the condition variable");
    exit(EXIT_FAILURE);
  }
}

/**
 * Frees the memory allocated for the processes queue, the string
 * in argVector, the argVector itself and the terminalsList.
 *
 * If mode is 1 it also prints the terminating info about all the processes
 * that were correctly endend.
 *
 * Doesn't have a return value.
 */
void exitFree (char **argVector, Queue processList, int mode, Queue terminalsList) {

	//while the list is not empty print the info of all processes
	while (!isEmptyQueue (processList)) {
		process_info process = (process_info) getFirstQueue(processList);

		// print the processes that terminated normally
		if (mode && exitedCorrectly(process))
			fprintf(stdout,
				"Process %d terminated normally: status=%d | execution time: %ld \n",
				getPid(process),
				getExitStatus(process),
				getEndTime(process) - getStartTime(process));
		// print the ones that didn't terminate normally
		else if (mode)
			fprintf(stdout,
					"Process %d terminated without calling exit.\n",
					getPid(process));
		freeProcInfo(process);		//free the process info struct
	}

	// free the pids in terminalsList
	while (!isEmptyQueue(terminalsList))
		free((pid_t *) getFirstQueue(terminalsList));

	// free memory
	freeQ(processList);
	freeQ(terminalsList);
	free(argVector[0]);
	free(argVector);
}

/**
 * Initializes the thread and the mutexes.
 *
 * thread_id is a pointer to a thread id.
 * Upon creation of the thread, the thread executes start_routine - start_routine
 * must not take any input arguments.
 *
 * mutex_id_list is a list of length n_mutexes of pointers to the mutexes' ids.
 */
void initThread (pthread_t* thread_id,
				 void* (*start_routine)(void*),
				 pthread_mutex_t* mutex_id_list[],
				 int n_mutexes) {

	// init the locks
	int lock_err, i;
	for (i = 0; i < n_mutexes; i++){

		if ((lock_err = pthread_mutex_init(mutex_id_list[i], NULL)) != 0)
			fprintf(stderr,
					"Error creating the lock number %d: %s\n",
					i+1, strerror(lock_err));
	}

	// init the thread
	int thread_err;
	if ((thread_err = pthread_create(thread_id, NULL, start_routine, NULL)) != 0){
		fprintf(stderr, "Error creating thread: %s\n", strerror(thread_err));
		exit(EXIT_FAILURE);
	}
}

/**
 * Terminates the monitor thread and destroys the locks.
 *
 * thread_id is the pointer to the thread's id to be terminated.
 * mutex_id_list is a list of length n_mutexes of pointers to the mutexes' ids.
 */
void exitThread (pthread_t* thread_id, pthread_mutex_t* mutex_id_list[], int n_mutexes){

	// waits for thread to terminate and when it does checks for errors
	int thread_err;
	if ((thread_err = pthread_join(*thread_id, NULL)) != 0)
		fprintf(stderr,
				"Error terminating thread: %s\n",
				strerror(thread_err));

	// Destroys the locks
	int lock_err, i;
	for (i = 0; i < n_mutexes; i++){
		if ((lock_err = pthread_mutex_destroy(mutex_id_list[i])) != 0)
			fprintf(stderr, "Error destroying the lock number %d: %s\n",
					i+1, strerror(lock_err));
	}
}

/**
 * Updates everything needed once a process terminates.
 *
 * Needs as inputs the process, it's end time and the status with which it ended.
 * It doesn't return anything.
 */
void updateTerminatedProcess (process_info process, time_t end_time, int status){

	//Store the time the process terminated
	setEndTime(process, end_time);

	//if the process exited store its exit status
	if (WIFEXITED(status))	setExitStatus(process, WEXITSTATUS(status));

	// the process didn't exit, so set an error in the end time
	else	setExitError(process);
}

/**
 * Determines if two processes are the same.
 * Two processes are the same if they have the same pid.
 * Takes as input a pointer to the pid and the pointer to a process.
 * Returns 1 if it is the same process and 0 otherwise.
 */
int compareProcesses (void* pid, void* process){

	return *(pid_t*) pid ==  getPid((process_info) process);

}

 /**
  * Reads the number of total iterarions and total execution time from log file.
  * Takes as inputs two pointers to integers to store the values and the log file
  * Returns 0 if the log file is corrupted
  */
int readLog(int *iterationsNumber, int *executionTime, FILE *logfile){
  // initialize the needed strings
  char iteration[MAXLINESIZE],
       pid[MAXLINESIZE],
       totalTime[MAXLINESIZE],
       aux[MAXLINESIZE];

  int corrupted = TRUE;

  // read all the lines until it reaches the end of the file
  // only stores the last 3 lines
  while (fgets(aux, MAXLINESIZE, logfile) != NULL){
    strcpy(iteration, aux);
    fgets(pid, MAXLINESIZE, logfile);
    fgets(totalTime, MAXLINESIZE, logfile);

    // if a line does not match the required format then the log file is corrupted
    if(testlines(iteration, pid, totalTime) != 0)
      corrupted = FALSE;
    else{
      corrupted = TRUE;
      break;
    }
  }

  // case the file is corrupted
  if(corrupted){
    iterationsNumber = 0;
    executionTime = 0;
    return 0;
  }
  // else get the required ints from the lines
  else{
    sscanf(iteration, ITERATION_FORMAT, iterationsNumber);
    (*iterationsNumber)++;
    sscanf(totalTime, EXECTIME_FORMAT, executionTime);
    return 1;
  }
}

/**
 * Writes to the log file the data of a terminated process
 * and updates the interation number and total execution time
 * Takes as inputs two pointers to the interation number and execution time,
 * a pointer to the process and the log file
 */
void writeLog(int *iterationNum, int *execTime, process_info process, FILE *logfile){
  // run time of the process
  int exectime = getEndTime(process) - getStartTime(process);

  // print the data to the file
  fprintf(logfile, ITERATION_FORMAT , (*iterationNum)++);
  fprintf(logfile, PID_FORMAT, getPid(process), exectime);
  fprintf(logfile, EXECTIME_FORMAT, *execTime += exectime);

  // flush the file
  xfflush(logfile);
}


/**
 * Tests if the given strings match the format specified for the log file lines
 * Returns 0 if they dont match
 */
int testlines(char *iteration, char *pid, char *exectime){
  // dummy variables
  int dummy1, dummy2;

  // tests if the strings match the required format
  if (sscanf(iteration, ITERATION_FORMAT, &dummy1) != 1 ||
      countTokens(iteration, TOKEN_DELIMITER) != 2 ||
      sscanf(pid, PID_FORMAT, &dummy1, &dummy2) != 2 ||
      countTokens(pid, TOKEN_DELIMITER) != 6 ||
      sscanf(exectime, EXECTIME_FORMAT, &dummy1) != 1 ||
      countTokens(exectime, TOKEN_DELIMITER) != 5)
    return 0;

  return 1;
}

/**
 * Counts the number of tokens delimited by the delimiters in a string
 * Returns the number of tokens
 */
int countTokens(char *str, const char *delim){
  int count = 0;
  char *ptr = str;
  while((ptr = strpbrk(ptr, delim)) != NULL) {
      count++;
      ptr++;
  }
  return count;
}

/**
 * Compares two pid_t referenced by pid1 and pid2.
 * Returns 0 if they are not equal.
 */
int comparePids(void *pid1, void *pid2){
	return *(pid_t*) pid1 == *(pid_t*) pid2;
}

/**
 * Kills all the terminals in the given list,
 * except the one with pid equal to callingPid
 */
void killTerminals(Queue terminalsList, pid_t callingPid){
	printf("Terminating all terminals\n");
 	pid_t *pid;
	while((pid = getFirstQueue(terminalsList)) != NULL){
		if (callingPid != *pid){ // do not kill the calling process
			printf("Terminating terminal with pid %d\n", *pid);
			kill(*pid, SIGTERM); // TODO check errors xkill
		}
		free(pid);
	}
}

/**
 * The handler for SIGINT
 */
void sigintHandler(int signal){
	sigintFlag = TRUE;
}
