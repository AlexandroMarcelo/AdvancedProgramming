/*
Created by Alexandro Marcelo González
*/
#ifndef RAIL_FENCE_CIPHER_TOOLS_H
#define RAIL_FENCE_CIPHER_TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1000

void menu();
FILE * openFile(char * file_name, char * mode);
void railFenceCipher(char * file_name, int num_rails, int option);
void printEncodedText(char ** text_in_rails, char * file_name, int num_rails, int length_text);
void printDecodedText(char ** text_in_rails, char * file_name, int num_rails, int length_text);
void encodeText(char ** text_in_rails, int num_rails, char * text, char * file_name);
char ** railsInitializer(int num_rails);
void decodeText(char ** text_in_rails, int num_rails, char * text, char * file_name);
char * cleanFile(char * file_name, char * prefix);
void releaseMemory(char ** matrix, int num_rails);

void createProcess();
void attendUser(int in_pipe[], int out_pipe[]);
void attendRequests(int in_pipe[], int out_pipe[]);
void openPipe(int pipe_channel[]);
void preparePipes(int in_pipe[], int out_pipe[]);
void closePipes(int in_pipe[], int out_pipe[]);

#endif
