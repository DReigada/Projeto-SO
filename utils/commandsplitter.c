/*
// Alteration to Command line reader
//
// Original code by:
// Sistemas Operativos, DEI/IST/ULisboa 2015-16
*/

#include <stdlib.h>
#include <string.h>

#include "commandsplitter.h"
/*
Reads up to 'vectorSize' space-separated arguments from given command
and saves them in the entries of the 'argVector' argument.
This function returns once enough arguments are read or the end of the line
the string is reached

Arguments:
 'argVector' should be a vector of char* previously allocated with
 as many entries as 'vectorSize'
 'vectorSize' is the size of the above vector. A vector of size N allows up to
 N-1 arguments to be read; the entry after the last argument is set to NULL.
 'command' should be a pointer to the string that stores the command to be parsed

Returns the number of arguments that were read.
*/
int readCommandArguments(char **argVector, int vectorSize, char* command)
{
  int numtokens = 0;
  char *s = " \n\t\r";

  int numRead;
  char *token;
  int i;

  if (argVector == NULL || vectorSize == 0)
    return 0;

  for (i = 0; i < vectorSize; i++)
    argVector[i] = NULL;

  // allocate str and copy command content to it
  char *str = malloc(strlen(command) + 1);
  strcpy(str, command);


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
