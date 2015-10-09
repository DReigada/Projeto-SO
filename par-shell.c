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
#include "Xmalloc.h"

#define MAX_N_INPUT 7 // the programs executed in the par-shell are limited to 
					  // 5 input arguments (the last entry is always set to NULL)


//TODO: MUDAR ISTO PARA ALGURES
int compareProcesses(void* pid, void* process){ 
	return *(pid_t*) pid ==  getPid((process_info) process);
}


int main(int argc, char* argv[]){

	// To initiate the par-shell no input arguments are needed
	if (argc != 1){
		printf("Usage: par-shell\n");
		exit(EXIT_FAILURE);
	}

	// allocates the memory for the command that the user inputs
	char** argVector = (char**) xmalloc(sizeof(char*) * MAX_N_INPUT); 			
	if (argVector == NULL){
		fprintf(stderr, "Error allocating argVector's memory. %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

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

		// exit
		if (strcmp(argVector[0], "exit") == 0){
			
			process_info process;

			printf("Waiting for all the processes to terminate\n");

			while(1){
				int status;
				pid_t child_pid = wait(&status);

				if (child_pid > 0){	// case the pid is valid 
					process = (process_info) getSpecificQueue(processList, &child_pid, compareProcesses, 0);
					//checks for an error on finding the element
					if (process == NULL){
						fprintf(stderr, "An error occurred when searching for a process in the list. Process not found.\n");
						continue;
					}
					else{
						setEndTime(process, 0); //NOT REALLY NECESSARY, Setting it to 0 to know it was terminated. 
						if (WIFEXITED(status))	//if the process exited store its exit status
							setExitStatus(process, WEXITSTATUS(status));					
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

			while(!isEmptyQueue(processList)){ //while the list is not empty print the info of all processes
				process = (process_info) getFirstQueue(processList);
				fprintf(stdout, "Process %d terminated with status %d\n", getPid(process), getExitStatus(process));
				freeProcInfo(process);		//free the process info struct
			}

			freeQ(processList);
			//free(argVector);
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
				fprintf(stderr, "Erro ao tentar abrir programa com o pathname. %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
		}

		// parent executes this
		else{
			process_info process = createProcessInfo(child_pid, time(NULL));
			addQueue(process, processList);  //add the created process to the list

			// clean the argVector file, to be ready to take another pathfile with different inputs
			// also free the memory allocated to store the string inputs
			//char* temp = argVector[0];
			for(int i = 0; argVector[i] != NULL; i++){
				argVector[i] = NULL;
			}

			//free(temp);
		}
	}
}








