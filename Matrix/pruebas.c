
/*
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>//for isDigit function
#include <unistd.h>


struct Matrix{
	char * fileName;
	int numRows;
	int numColumns;
	float ** matrix;
};

bool isFloat(char * givenNumber);
bool isInteger(char * givenNumber);
FILE * openFile(char *argv, char * mode);
void readFile(char * fileName, struct Matrix * matrixPtr);
void closeFile(FILE *file);


int main(int argc, char * argv[])
{
	int c;
	char * file1 = NULL;//save the -i argument
	char * file2 = NULL;//save the -o argument
	char * result = NULL;//save the -o argument
	
 	while ((c = getopt (argc, argv, "1:2:3:")) != -1){//to obtain the arguments in terminal. Source: https://pubs.opengroup.org/onlinepubs/009696799/functions/getopt.html
	    switch (c)
    	{
    		case '1':
        		//printf("The input base value is: %s\n", argv[2]);
        		file1 = optarg;
        		printf("%s\n", file1);
        		break;
      		case '2':
        		//printf("The output base value is: %s\n", argv[4]);
        		file2 = optarg;
        		printf("%s\n", file1);
        		break;
        	case '3':
        		//printf("The output base value is: %s\n", argv[4]);
        		result = optarg;
        		printf("%s\n", result);
        		break;
	      	default:
	        	printf("Arguments have to be correct. Run it again\n");
	        	return 0;
	    }
	}


 	struct Matrix matrix1;
 	matrix1.fileName = argv[1];
 	//readFile(matrix1.fileName, &matrix1.numRows);
 	readFile(matrix1.fileName, &matrix1);
 	printf("%f\n",matrix1.matrix[2]);

 	struct Matrix matrix2;
 	matrix2.fileName = argv[2];
 	readFile(matrix2.fileName, &matrix2);

	//readFile(argv[1]);
	return 0;
}

FILE * openFile(char * fileName, char * mode){
	FILE * file = NULL;
	file = fopen (fileName, mode);//opening and reading the given file
	if (file == NULL)	{
		printf("Error while opening the file. Make sure you pass correctly the name through arguments while compiling.\n");
		exit(0);
	}
	return file;
}

void readFile(char * fileName, struct Matrix * matrixPtr){
	FILE * file = NULL;
	file = openFile(fileName, "r");
	char *rows = (char *)malloc(sizeof(char));
 	char *columns = (char *)malloc(sizeof(char));
 	char *auxArray = (char *)malloc(sizeof(char));
 	//float matrixPtr->numColumns;//0.0F
 	//float matrixPtr->numRows;

 	fscanf(file, "%s", rows);
 	fscanf(file, "%s", columns);
 	printf("The number of rows are: %s.\n", rows);
 	printf("The number of columns are: %s.\n", columns);

 	if(isInteger(rows) == false || isInteger(columns) == false){
 		printf("You should use only integers for the size of the matrix in the given file.\n");
 		exit(0);
 	}
 	matrixPtr->numColumns = atoi(columns);
 	matrixPtr->numRows = atoi(rows);
 	matrixPtr->matrix = (float *)calloc((matrixPtr->numRows * matrixPtr->numColumns), sizeof(float));
 	int count = 0;

 	while(fscanf(file, "%s", auxArray)!=EOF){
 		if (isFloat(auxArray) == true){
 			matrixPtr->matrix [count] = atof(auxArray);
 			printf("%0.2f ",matrixPtr->matrix [count]);
 			count++;
 		}
 		else{
 			printf("The file must contain only float data type. Resolve it and run it again.\n");
 			exit(0);
 		}
	}
	printf("\n");
	printf("\n");
 	free(rows);
 	free(columns);
 	//free(matrix1);
 	free(auxArray);
 	fclose(file);
}

void closeFile(FILE *file){
	fclose(file);
}

bool isInteger(char * givenNumber){
	for (int i = 0; i < strlen(givenNumber); i++)	{
		if(!isdigit(givenNumber[i])){
			return false;
	    }
	}
	return true;
}

bool isFloat(char * givenNumber){
	int cont = 0;
	for (int i = 0; i < strlen(givenNumber); i++)	{
		if(!isdigit(givenNumber[i])){
			if (givenNumber[i] == '.' && cont == 0)			{
				cont++;
			}
			else{
				return false;
			}
	    }
	}
	return true;
}

void matrixMultiplication(float * matrix1, float matrix2){

}
*/

/*
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>//for isDigit function

struct Matrix{
	char * fileName;
	int numRows;
	int numColumns;
	float * matrix;
};

bool isFloat(char * givenNumber);
bool isInteger(char * givenNumber);
FILE * openFile(char *argv, char * mode);
void readFile(char * fileName, struct Matrix * matrixPtr);
void closeFile(FILE *file);


int main(int argc, char * argv[])
{
 	
 	struct Matrix matrix1;
 	matrix1.fileName = argv[1];
 	//readFile(matrix1.fileName, &matrix1.numRows);
 	readFile(matrix1.fileName, &matrix1);
 	printf("%f\n",matrix1.matrix[2]);

 	struct Matrix matrix2;
 	matrix2.fileName = argv[2];
 	readFile(matrix2.fileName, &matrix2);

	//readFile(argv[1]);
	return 0;
}

FILE * openFile(char * fileName, char * mode){
	FILE * file = NULL;
	file = fopen (fileName, mode);//opening and reading the given file
	if (file == NULL)	{
		printf("Error while opening the file. Make sure you pass correctly the name through arguments while compiling.\n");
		exit(0);
	}
	return file;
}

void readFile(char * fileName, struct Matrix * matrixPtr){
	FILE * file = NULL;
	file = openFile(fileName, "r");
	char *rows = (char *)malloc(sizeof(char));
 	char *columns = (char *)malloc(sizeof(char));
 	char *auxArray = (char *)malloc(sizeof(char));
 	//float matrixPtr->numColumns;//0.0F
 	//float matrixPtr->numRows;

 	fscanf(file, "%s", rows);
 	fscanf(file, "%s", columns);
 	printf("The number of rows are: %s.\n", rows);
 	printf("The number of columns are: %s.\n", columns);

 	if(isInteger(rows) == false || isInteger(columns) == false){
 		printf("You should use only integers for the size of the matrix in the given file.\n");
 		exit(0);
 	}
 	matrixPtr->numColumns = atoi(columns);
 	matrixPtr->numRows = atoi(rows);
 	matrixPtr->matrix = (float *)calloc((matrixPtr->numRows * matrixPtr->numColumns), sizeof(float));
 	int count = 0;

 	while(fscanf(file, "%s", auxArray)!=EOF){
 		if (isFloat(auxArray) == true){
 			matrixPtr->matrix [count] = atof(auxArray);
 			printf("%0.2f ",matrixPtr->matrix [count]);
 			count++;
 		}
 		else{
 			printf("The file must contain only float data type. Resolve it and run it again.\n");
 			exit(0);
 		}
	}
	printf("\n");
	printf("\n");
 	free(rows);
 	free(columns);
 	//free(matrix1);
 	free(auxArray);
 	fclose(file);
}

void closeFile(FILE *file){
	fclose(file);
}

bool isInteger(char * givenNumber){
	for (int i = 0; i < strlen(givenNumber); i++)	{
		if(!isdigit(givenNumber[i])){
			return false;
	    }
	}
	return true;
}

bool isFloat(char * givenNumber){
	int cont = 0;
	for (int i = 0; i < strlen(givenNumber); i++)	{
		if(!isdigit(givenNumber[i])){
			if (givenNumber[i] == '.' && cont == 0)			{
				cont++;
			}
			else{
				return false;
			}
	    }
	}
	return true;
}

void matrixMultiplication(float * matrix1, float matrix2){
	
}



*/



/*

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>//for isDigit function

bool isFloat(char * givenNumber);
bool isInteger(char * givenNumber);
FILE * openFile(char *argv, char * mode);
void readFile(char *argv);
void closeFile(FILE *file);

int main(int argc, char * argv[])
{
 	
 	

 	
 	//char *size = malloc(5);
 	


 	 //free(matrix1);
	//fclose(file);//closing the file


	//openFile(file, argv);
	readFile(argv[1]);
	return 0;
}

FILE * openFile(char * fileName, char * mode){
	
	FILE * file = NULL;
	file = fopen (fileName, mode);//opening and reading the given file
	if (file == NULL)	{
		printf("Error while opening the file. Make sure you pass correctly the name through arguments while compiling.\n");
		exit(0);
	}
	return file;
}

void readFile(char * fileName){
	FILE * file = NULL;
	file = openFile(fileName, "r");
	char *rows = (char *)malloc(sizeof(char));
 	char *columns = (char *)malloc(sizeof(char));
 	char *auxArray = (char *)malloc(sizeof(char));
 	float numRows, numColumns;//0.0F

 	fscanf(file, "%s", rows);
 	fscanf(file, "%s", columns);
 	printf("The number of rows are: %s.\n", rows);
 	printf("The number of columns are: %s.\n", columns);

 	if(isInteger(rows) == false || isInteger(columns) == false){
 		printf("You should use only integers for the size of the matrix in the given file.\n");
 		exit(0);
 	}
 	numColumns = atoi(columns);
 	numRows = atoi(rows);
 	float *matrix1 = (float *)calloc((numRows * numColumns), sizeof(float));
 	int count = 0;

 	while(fscanf(file, "%s", auxArray)!=EOF){
 		if (isDigit(auxArray) == true){
 			matrix1[count] = atof(auxArray);
 			printf("%0.2f ",matrix1[count]);
 			count++;
 		}
 		else{
 			printf("The file must contain only float data type. Resolve it and run it again.\n");
 			exit(0);
 		}
	}
	printf("\n");
 	free(rows);
 	free(columns);
 	free(matrix1);
 	free(auxArray);
 	fclose(file);
}

void closeFile(FILE *file){
	fclose(file);
}

bool isInteger(char * givenNumber){
	for (int i = 0; i < strlen(givenNumber); i++)	{
		if(!isdigit(givenNumber[i])){
			return false;
	    }
	}
	return true;
}

bool isFloat(char * givenNumber){
	int cont = 0;
	for (int i = 0; i < strlen(givenNumber); i++)	{
		if(!isdigit(givenNumber[i])){
			if (givenNumber[i] == '.' && cont == 0)			{
				cont++;
			}
			else{
				return false;
			}
	    }
	}
	return true;
}
*/