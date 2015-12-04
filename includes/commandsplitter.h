/*
// Alteration to Command line reader
//
// Original code by:
// Sistemas Operativos, DEI/IST/ULisboa 2015-16
*/

#ifndef _COMMANDSPLITTER_H_
#define _COMMANDSPLITTER_H_

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
int readCommandArguments(char **argVector, int vectorSize, char* command);

#endif
