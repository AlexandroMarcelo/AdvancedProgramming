#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct matrix_struct{
	int rows;
	int cols;
	float ** data;
}matrix_t;

void createProcess();
void preparePipes(int in_pipe[], int out_pipe[]);
void closePipes(int in_pipe[], int out_pipe[]);
void parentLoop(int in_pipe[], int out_pipe[]);
void childLoop(int in_pipe[], int out_pipe[]);

int main()
{
	createProcess();
	return 0;
}

void createProcess(){
	pid_t new_pid;

	int parent_to_child[2];
	int child_to_parent[2];
	if (pipe(parent_to_child) == -1)
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}
	if (pipe(child_to_parent) == -1)
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	new_pid = fork();

	if (new_pid > 0)
	{
		parentLoop(child_to_parent, parent_to_child);
	}
	else if (new_pid == 0)
	{
		childLoop(parent_to_child, child_to_parent);
	}
	else
	{
		printf("ERROR: Unable to fork.\n");
		exit(EXIT_FAILURE);
	}
}

//Close the end of the pipes that will not be used
void preparePipes(int in_pipe[], int out_pipe[]){
	close(in_pipe[1]);
	close(out_pipe[0]);
}

void closePipes(int in_pipe[], int out_pipe[]){
	close(in_pipe[0]);
	close(out_pipe[1]);
}

void parentLoop(int in_pipe[], int out_pipe[]){
	preparePipes(in_pipe, out_pipe);
	while(1){
		matrix
	}
	closePipes(in_pipe, out_pipe);
}

void childLoop(int in_pipe[], int out_pipe[]){
	preparePipes(in_pipe, out_pipe);
	while(1){
		matrix_t matrix;
		matrix.rows = 3;
		matrix.cols = 3;
		for (int i = 0; i < matrix.rows; i++)
		{
			for (int i = 0; i < matrix.cols; i++)
			{
				
			}
		}
	}
	closePipes(in_pipe, out_pipe);
}


