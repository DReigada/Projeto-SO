/* Function to get a line from stdin */

#include <stdio.h>
#include <string.h>

#include "getLine.h"

/**
 * It is the same as the fgets (it uses it) with the only difference being that
 * it restricts the input stream to stdin and tests if the string that was read
 * is NULL and if there was overflow.
 *
 * Returns 1 if successful; 0 if there was no input or an error ocurred using
 * fgets; -1 if there was overflow
 */
int getLine(char* str, int sz){
  char c;

  if (fgets(str, sz, stdin) == NULL){
    return 0;
  }

  int len = strlen(str);

  //check if there was overflow
  if (str[len - 1] != '\n'){

    int feedback = 1;
    //if there was overflow, flush the excess so it doesnt influence future readings
    while ((c = getchar() != '\n') && (c != EOF)){
      feedback = -1;
    }

    str[len - 1] = '\0';
    return feedback;

  } else if (len == 1) return 0;

  // remove new line
  str[len - 1] = '\0';

  return 1;
}
