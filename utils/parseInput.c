/*
// Command line reader (header file), version 2
// Sistemas Operativos, DEI/IST/ULisboa 2015-16
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define TOO_LONG -1
#define EMPTY_INPUT 0
#define OK 1

/* 
Reads up to 'vectorSize' space-separated arguments from the string str
and saves them in the entries of the 'argVector' argument.
This function returns once enough arguments are read or the end of the line 
is reached

Arguments: 
 'str' should be a str 
 'argVector' should be a vector of char* previously allocated with
 as many entries as 'vectorSize'
 'vectorSize' is the size of the above vector. A vector of size N allows up to 
 N-1 arguments to be read; the entry after the last argument is set to NULL.

Return value:
 The number of arguments that were read, or -1 if some error occurred.
*/
int getArguments(char* str, char **argVector, int vectorSize)
{
  int numtokens = 0;
  char *s = " \n\t\r";

  int i;

  char *token;

  if (argVector == NULL || vectorSize == 0 || str == NULL)
    return -1;
   
  /* get the first token */
  token = strtok(str, s);
   
  /* walk through other tokens */
  while( numtokens < vectorSize-1 && token != NULL ) {
    argVector[numtokens] = token;
    numtokens ++;
    
    token = strtok(NULL, s);
  }
  
  if (numtokens == 0){
    free(str);
    return 0;
  }

  for (i = numtokens; i<vectorSize; i++) {
    argVector[i] = NULL;
  }
   
  return numtokens;
}


/**
 * It is the same as the fgets (it uses it) with the only difference being that
 * it restricts the input stream to stdin and tests if the string that was read
 * is NULL and if there was overflow.
 *
 * Returns 1 if successful; 0 if there was no input or an error ocurred using 
 * fgets; -1 if there was overflow
 */
int getLine(char* str, size_t sz){
  char c;

  fgets(str, sz, stdin);

  if (str == NULL){
    return EMPTY_INPUT;
  }

  int len = strlen(str);

  //check if there was overflow 
  if (str[len - 1] != '\n'){

    int feedback = OK;
    //if there was overflow, flush the excess so it doesnt influence future readings
    while ((c = getchar() != '\n') && (c != EOF)){
      feedback = TOO_LONG;
    }

    str[len - 1] = '\0';
    return feedback;
  }

  // remove new line
  str[len - 1] = '\0';

  return OK;
}

