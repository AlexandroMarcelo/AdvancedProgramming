/*
Alexandro Francisco Marcelo Gonzalez A01021383

01/03/2019
*/
#ifndef SOCKETS_TOOLS_H
#define SOCKETS_TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Sockets libraries
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Function declarations
int prepareServer(char * port, int max_queue);
int connectToServer(char * address, char * port);
int recvMessage(int connection_fd, void * buffer, int buffer_size);

#endif