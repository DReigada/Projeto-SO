/* Function to get a line from stdin */

#include <stdio.h>
#include <string.h>

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

  } else if (len == 1) return EMPTY_STRING;

  // remove new line
  str[len - 1] = '\0';

  return OK;
}