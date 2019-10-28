/*
Created by Alexandro Marcelo González A01021383
*/

#include "rail_fence_cipher_tools.h"

void menu(){
    pid_t pid;
    int pipe_parent_child[2];
    int pipe_child_parent[2];

    //for comunicate the child and parent
    openPipe(pipe_parent_child);
    openPipe(pipe_child_parent);

    pid = fork();//creating the child

    if (pid > 0){//for the parent process
        attendUser(pipe_child_parent, pipe_parent_child);
    }
    else if (pid == 0){// for the child process
        attendRequests(pipe_parent_child, pipe_child_parent);
    }
    else{
        perror("Unable to create a new process\n");
        exit(EXIT_FAILURE);
    }   
}

//for opening the file
FILE * openFile(char * file_name, char * mode){
    FILE * file = NULL;
    file = fopen(file_name, mode);
    if (file == NULL) {
        printf("The file could not be opened.\n");
        exit(0);
    }
    return file;
}

//for create encode or decode for the given file
void railFenceCipher(char * file_name, int num_rails, int option){
    char *f = (char *)malloc(sizeof(char)*10);

    if (option == 1) {//if the option given by the user is 1 is for encode
        f = "encoded_";
    }
    else if (option == 2){//if the option given by the user is 2 is for decode
        f = "decoded_";
    }
    printf("Creating the caca : %s\n", file_name);
    char * output_file = (char *) malloc(sizeof(char)*500);//allocating the memory for store the name of the output file from the encoded/decoded
    output_file = cleanFile(file_name, f);//jus for clean the file if it exist
    char ** text_in_rails;
    text_in_rails = railsInitializer(num_rails);//creating the rails for the encode/decode
    printf("Creating the file : %s\n", output_file);
    //printf("The given file is(cleaning): %s\n", output_file);
    char *line = (char *)malloc(sizeof(char)*1000);
    FILE * file = openFile(file_name, "r");//opening the given file for encode/decode
    size_t len = 0;//for store each line from the file
    ssize_t read;//for ensure the given line is not null
    while((read = getline(&line, &len, file)) != -1){//to obtain line per line from the file: https://stackoverflow.com/questions/3501338/c-read-file-line-by-line
        //if is for encode
        if (option == 1){
            encodeText(text_in_rails,num_rails,line, output_file);
        }
        else if (option == 2){//if is for decode
            decodeText(text_in_rails, num_rails, line, output_file);
        }
    }
    //for release the allocated memory
    releaseMemory(text_in_rails, num_rails);
    free(output_file);
    free(line);
    //free(f);
}
void releaseMemory(char ** matrix, int num_rails){
	for (int i = 0; i < num_rails; i++){
		free(matrix[i]);//free the assigned memory for the array in each slot from the father array
	}
	free(matrix);//free the memory for the array of arrays
}

char * cleanFile(char * file_name, char * prefix){
    char * f_name = (char *) malloc(sizeof(char)*100);
    strcpy(f_name, prefix);
    strcat(f_name, file_name);
    FILE * clean_file = openFile(f_name, "w");
    fclose(clean_file);
    return f_name;
}

void encodeText(char ** text_in_rails, int num_rails, char * text, char * file_name){
    int cont = 0;//for know the size of the string
    int positionRail = 0;//for the position of each rail
    int aux = 1;//for the direction of the rails
    while(text[cont] != 10 && text[cont] != 0){
        cont++;//counting the string (size)
    }
    for(int i = 0; i < num_rails; ++i){
        for(int j = 0; j < cont; j++){
            text_in_rails[i][j] = '\n';//putting \n in each position of the rails
        }
    }
    for(int i = 0; i < cont; i++){
        text_in_rails[positionRail][i] = text[i];//storing the extracted line from the file to the rails
        if (positionRail == num_rails-1){
            aux = -1;//jus for turning the direction of the rail
        }
        else if (positionRail == 0) {
            aux = 1;
        }
        positionRail = positionRail + aux;
    }
    printEncodedText(text_in_rails, file_name, num_rails, cont);
}

char ** railsInitializer(int num_rails){//For allocate the memory of the matrix (rails)
 	char ** matrix = (char **)calloc(num_rails*100, sizeof(char));//declaring the array of array dynamically
	for(int i = 0; i < num_rails; i++){
		matrix[i] = (char *)calloc(num_rails*100, sizeof(char));//declaring the array dynamically
 	}
	return matrix;
}

void decodeText(char ** text_in_rails, int num_rails, char * text, char * file_name){
    int cont = 0;//for know the size of the string
    int positionRail = 0;//for the position of each rail
    int aux = 1;//for the direction of the rails
    while(text[cont] != 10 && text[cont] != 0){
        cont++;//counting the string (size)
    }
    for(int i = 0; i < num_rails; ++i){
        for(int j = 0; j < cont; j++){
            text_in_rails[i][j] = '\n';//putting \n in each position of the rails
        }
    }
    for(int i = 0; i < cont; i++){
        text_in_rails[positionRail][i] = '*';//storing a * in the position where the encoded text going to insert later on
        if (positionRail == num_rails-1){
            aux = -1;//jus for turning the direction of the rail
        }
        else if (positionRail == 0) {
            aux = 1;
        }
        positionRail = positionRail + aux;
    }
    positionRail = 0;
    int pos = 0;
    for(int i = 0; i < num_rails; i++){
        for(int j = 0; j < cont; j++){
            if (text_in_rails[i][j] == '*') {//storing each position of the encoded text where there are *
                text_in_rails[i][j] = text[pos];
                pos++;
            }
        }
    }
    printDecodedText(text_in_rails, file_name, num_rails, cont);
}

void printDecodedText(char ** text_in_rails, char * file_name, int num_rails, int length_text){
    FILE * file = openFile(file_name, "a+");//opening the output file. a+= append to the file without errase the content while opening
    int positionRail = 0;//for the position of the rail
    int direction = 1;//change direction of the rails
    for(int i = 0; i < length_text; i++){
        fprintf(file, "%c", text_in_rails[positionRail][i]);//printing each position in the file
        if (positionRail == num_rails-1){
            direction = -1;//changing direction
        }
        else if (positionRail == 0){
            direction = 1;
        }
        positionRail = positionRail + direction;
    }
    fprintf(file, "\n");
    fclose(file);//closign file
}

void printEncodedText(char ** text_in_rails, char * file_name, int num_rails, int length_text){
    FILE * file = openFile(file_name, "a+");//opening the file with a+= append to the file without errase the content while opening
    for(int i = 0; i < num_rails; i++){
        for(int j = 0; j < length_text; j++){
            if(text_in_rails[i][j] != '\n'){//if there is not a \n print the character on this position in the file
                fprintf(file, "%c", text_in_rails[i][j]);//printing in file
            }
        }
    }
    fprintf(file, "\n");
    fclose(file);//closing the file
}

// Main loop for the parent process, listens for the user requests
// Receive: the file pointers to write and read to the child
void attendUser(int in_pipe[], int out_pipe[])
{
    int num_rails = 2;//for the number of rails (key)
    int option = 1;//for encode or decode
    char buffer[BUFFER_SIZE];//the buffer where the parent comunicates with its child
    char file_name[BUFFER_SIZE] = {0};//for the file name where the text are

    preparePipes(in_pipe, out_pipe);//openidn the channels
    while (1)//always listening
    {
        printf("Welcome to the program \"Rail_fence_cipher\"\n");
        printf("Type what you want:\n0. To exit.\n1. To Encode a file.\n2. To Decode a file.\n");
        scanf("%d", &option);
        printf("Give me the file where the text are.\n");
        scanf("%s", file_name);
        printf("Give me the number of rails.\n");
        scanf("%d", &num_rails);
        sprintf(buffer, "%d*%s*%d*", option, file_name, num_rails);

        write(out_pipe[1], buffer, BUFFER_SIZE);//writing in the pipe for comunicate to its child
        read(in_pipe[0], buffer, BUFFER_SIZE);//reading the name of the file given by the child

        printf("The file sent from child is: %s\n", buffer);//printing what the child sent
    }
    closePipes(in_pipe, out_pipe);//closing the channels
}

// Loop for the child process to wait for requests from the parent
// Receive: the file pointers to write and read to the parent
void attendRequests(int in_pipe[], int out_pipe[])
{
    int num_rails = 0;
    int option = 1;
    //char * file_name = NULL;

    char buffer[BUFFER_SIZE];
    char numRails[BUFFER_SIZE];
    char reply[BUFFER_SIZE];
    char file_name[BUFFER_SIZE] =  {0};
    preparePipes(in_pipe, out_pipe);//openidn the channels
    
    while (1)
    {
        int i = 2;
        int cont = 0;
        
        read(in_pipe[0], buffer, BUFFER_SIZE);
        if (buffer[0] == '1'||buffer[0] == '2') {
            //file_name = (char*)malloc(strlen(buffer)*10 * sizeof(char));
            //strcpy(file_name, "0");
            while(buffer[i] != '*'){
                file_name[cont] = buffer[i];
                i++;
                cont++;
            }
            file_name[cont] = '\0';
            i++;
            cont = 0;
            while(buffer[i] != '*'){
                numRails[cont] = buffer[i];
                i++;
                cont++;
            }
            num_rails = atoi(numRails);
            option = buffer[0] - '0';
             printf("file_name child: %s\n", file_name);
            railFenceCipher(file_name,num_rails,option);
            i = 2;
            cont = 0;
            //free(file_name);
        }
        else if (buffer[0] == '0'){
            printf("Thank you for used the program.\n");
            exit(EXIT_SUCCESS);
        }
        else{
            printf("You should give the correct option.\n");
            exit(EXIT_FAILURE);
        }
       
        //printf("From my parent: %s\n", buffer);
        printf("Exit from child.\n");
        if (buffer[0] == '1'){
            sprintf(reply,"encoded_%s", file_name);
        }
        else if (buffer[0] == '2'){
            sprintf(reply,"decoded_%s", file_name);
        }
        write(out_pipe[1], reply, BUFFER_SIZE);//writing the output file to its parent
    }
    closePipes(in_pipe, out_pipe);//closing the channels
    exit(EXIT_SUCCESS);
}

//opening the child-parent comunication
void openPipe(int pipe_channel[]){
    if (pipe(pipe_channel) == -1){
        perror("Unable to create pipe\n");
        exit(EXIT_FAILURE);
    }
}

//for prepare the channels of
void preparePipes(int in_pipe[], int out_pipe[]){
    close(in_pipe[1]);
    close(out_pipe[0]);
}

void closePipes(int in_pipe[], int out_pipe[])
{
    close(in_pipe[0]);
    close(out_pipe[1]);
}
