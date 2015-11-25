/*
// Command line reader (header file), version 2
// Sistemas Operativos, DEI/IST/ULisboa 2015-16
*/

#ifndef COMMANDLINEREADER_H
#define COMMANDLINEREADER_H

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
 The number of arguments that were read, or -1 if some error occurred.
*/

int getArguments(char **argVector, int vectorSize);

/**
 * It is the same as the fgets (it uses it) with the only difference being that
 * it restricts the input stream to stdin and tests if the string that was read
 * is NULL and if there was overflow.
 *
 * Returns 1 if successful; 0 if there was no input or an error ocurred using 
 * fgets; -1 if there was overflow
 */
int getLine(char* str, size_t sz);

#endif



