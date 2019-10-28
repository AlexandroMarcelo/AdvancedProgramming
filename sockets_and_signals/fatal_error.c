/*
Alexandro Fracisco Marcelo Gonzalez A01021383
17/03/2019
Based on Gilberto Echeverria code
*/

#include "fatal_error.h"

void fatalError(const char * message)
{
    perror(message);
    exit(EXIT_FAILURE);
}
