/*
// Command line reader (header file), version 2
// Sistemas Operativos, DEI/IST/ULisboa 2015-16
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

/*
Reads up to 'vectorSize' space-separated arguments from the standard input
and saves them in the entries of the 'argVector' argument.
This function returns once enough arguments are read or the end of the line
is reached

Arguments:
 'argVector' should be a vector of char* previously allocated with
 as many entries as 'vectorSize'
 'vectorSize' is the size of the above vector. A vector of size N allows up to
 N-1 arguments to be read; the entry after the last argument is set to NULL.

Return value:
 The number of arguments that were read, or:
    -1 if some error occurred.
    -2 is returned then read returned EOF.
    -3 if the read call was interrupted by a signal before any data was read

*/

int readLineArguments(char **argVector, int vectorSize)
{
  int numtokens = 0;
  char *s = " \n\t\r";

  char *str = malloc(BUFSIZ);
  int numRead;
  int i;

  char *token;

  if (argVector == NULL || vectorSize == 0)
    return 0;

  for (int i = 0; i < vectorSize; i++)
    argVector[i] = NULL;

  if ((numRead = read(STDIN_FILENO, str, BUFSIZ)) < 0) {
    if (errno == EINTR) {
      free(str);
      return -3;
    }
    fprintf(stderr, "Error reading %s\n", strerror(errno));
    free(str);
    return -1;
  }
  // if read returned 0 it reached EOF
  if (numRead == 0) {
    free(str);
    return -2;
  }

  str[numRead] = '\0';
  // reallocs the string to its size
  str = realloc(str, numRead + 1);

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
