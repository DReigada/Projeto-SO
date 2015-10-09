/* Projeto de SO */

// includes "standards"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>


// related to the system calls functions
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>

// our own files
#include "commandlinereader.h"
#include "QUEUE.h"
#include "process_info.h"
#include "Auxiliares.h"

#define MAX_N_INPUT 7 // the programs executed in the par-shell are limited to 
					  // 5 input arguments (the last entry is always set to NULL)


int main(int argc, char* argv[]){

	// To initiate the par-shell no input arguments are needed
	if (argc != 1){
		printf("Usage: par-shell\n");
		exit(EXIT_FAILURE);
	}

	// allocates the memory for the command that the user inputs
	char** argVector = (char**) xmalloc(sizeof(char*) * MAX_N_INPUT); 			

	// stores the number of arguments from the user
	int narg = 0;

	// initialize the list to store the processes
	Queue processList = initQueue();

	// Continue until the exit command is executed
	while (1){
		
		// read the user input
		narg = readLineArguments(argVector, MAX_N_INPUT);

		// checks for errors reading the input and reads again if there were
		if (narg == -1){
			fprintf(stderr, "Some error occurred reading the user's input.\n");
			continue;
		}
		else if (narg == 0){ //in case no command was inputted
			fprintf(stdout, "Please input a valid command\n");
			continue;
		}

		/* see what command the user typed */

		// exit command
		if (strcmp(argVector[0], "exit") == 0){

			// initialize the process struct
			process_info process;

			printf("Waiting for all the processes to terminate\n");

			// wait to finish all the processes that were started
			while(1){

				int status;
				pid_t child_pid = wait(&status);

				if (child_pid > 0){	// case the pid is valid 

					// get the process that finished from the queue
					process = (process_info) getSpecificQueue(processList, &child_pid, compareProcesses, 0);

					//checks for an error on finding the element
					if (process == NULL){
						fprintf(stderr, "An error occurred when searching for a process in the list. Process not found.\n");
						continue;
					}
					else{
						setEndTime(process, 0); //NOT REALLY NECESSARY, Setting the end time to 0 to know it was terminated.

						if (WIFEXITED(status))	//if the process exited store its exit status
							setExitStatus(process, WEXITSTATUS(status));	

						else	// the process didn't correctly ended, so set an error in the end time
							setPidError(process);			
					}
				}
				else{ 
					if (errno == ECHILD){	//checks to see if the error that wait() return was because there were no child processes
						break;
					}
					else{					//if not prints the error
						fprintf(stderr, "An error occurred when wating for a process to exit. %s\n", strerror(errno));
						break;
					}
				}
			}

			// prints the final results and frees the memory allocated
			exitFree(argVector, processList, 1);

			// exit the shell with success
			exit(EXIT_SUCCESS);
		}

		// else we assume it was given a path to a program to execute
		pid_t child_pid = fork();

		// check for errors in the creation of a new process
		if (child_pid == -1){
			fprintf(stderr, "Some error occurred when creating a new process. %s\n", strerror(errno));
			continue;
		}

		// child executes this
		if (child_pid == 0){

			// Change the process image to the program given by the user
			int err = execv(argVector[0], argVector);

			// Check for errors
			if (err == -1){
				fprintf(stderr, "Erro ao tentar abrir programa com o pathname. %s\n", strerror(errno)); //print error message

				// free the allocated memory that was copied for the child process
				exitFree(argVector, processList, 0);

				exit(EXIT_FAILURE); //exits
			}
		}

		// parent executes this
		else{

			//add the created process to the list
			process_info process = createProcessInfo(child_pid, time(NULL));
			addQueue(process, processList);  

			//free the memory allocated to store new commands
			free(argVector[0]);
		}
	}
}



