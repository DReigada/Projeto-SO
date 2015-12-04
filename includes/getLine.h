/* Function to get a line from stdin */

/**
 * It is the same as the fgets (it uses it) with the only difference being that
 * it restricts the input stream to stdin and tests if the string that was read
 * is NULL and if there was overflow.
 *
 * Returns 1 if successful; 0 if there was no input or an error ocurred using 
 * fgets; -1 if there was overflow
 */
int getLine(char* str, int sz);


