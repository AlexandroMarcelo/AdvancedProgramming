/*
Alexandro Francisco Marcelo Gonzalez A01021383
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

struct Matrix{//struct for facilitate the manipulation of the matrices
	char * fileName;
	int numRows;
	int numColumns;
	float ** matrix;
};

bool isFloat(char * givenNumber);//for checking each value in the given file
bool isInteger(char * givenNumber);//for checking the size (number of rows and columns) in the given file
FILE * openFile(char *argv, char * mode);//for opening files
void readFileAndStoreMatrix(char * fileName, struct Matrix * matrixPtr);
float ** matrixMultiplication(struct Matrix * matrix1, struct Matrix * matrix2);
bool checkCanMultiply(struct Matrix * matrix1, struct Matrix * matrix2);
void printInTheGivenFile(char * fileName, float ** result, int numRows, int numColumns);
float ** matrixInitializer(int numberOfRows, int numberOfColumns);
void releaseMemory(float ** matrix, int numberOfRows);

int main(int argc, char * argv[])
{
	int c;
	char * file1 = NULL;//save the -i argument
	char * file2 = NULL;//save the -o argument
	char * outputFile = NULL;//save the -o argument
	
 	while ((c = getopt (argc, argv, "1:2:r:")) != -1){//to obtain the arguments in terminal. Source: https://pubs.opengroup.org/onlinepubs/009696799/functions/getopt.html
	    switch (c)
    	{
    		case '1':
        		file1 = optarg;
        		break;
      		case '2':
        		file2 = optarg;
        		break;
        	case 'r':
        		outputFile = optarg;
        		break;
	      	default:
	        	printf("Arguments have to be correct. Run it again\n");
	        	return 0;
	    }
	}
 	
 	struct Matrix matrix1;//declaring a struct for the first matrix
 	matrix1.fileName = file1;
 	readFileAndStoreMatrix(matrix1.fileName, &matrix1);

 	struct Matrix matrix2;//declaring a struct for the second matrix
 	matrix2.fileName = file2;
 	readFileAndStoreMatrix(matrix2.fileName, &matrix2);

 	float **result = matrixInitializer(matrix1.numRows, matrix2.numColumns);//just initializing dynamically the array of array for the result

 	result = matrixMultiplication(&matrix1, &matrix2);//storing the outcome from the mulipication
	
	printInTheGivenFile(outputFile, result, matrix1.numRows, matrix2.numColumns);//printing the outcome in the given file
	return 0;
}

FILE * openFile(char * fileName, char * mode){
	FILE * file = NULL;
	file = fopen (fileName, mode);//opening and reading/writing the given file
	if (file == NULL)	{
		printf("Error while opening the file. Make sure you pass correctly the name through arguments while compiling.\n");
		exit(0);
	}
	return file;
}

void readFileAndStoreMatrix(char * fileName, struct Matrix * matrixPtr){
	FILE * file = NULL;
	file = openFile(fileName, "r");
	char *rows = (char *)malloc(sizeof(char));//for store the value from the file as char dynamically
 	char *columns = (char *)malloc(sizeof(char));
 	char *auxArray = (char *)malloc(sizeof(char));
 	int count = 0, posRows = 0, posColumns = 0;

 	fscanf(file, "%s", rows);
 	fscanf(file, "%s", columns);
 	//printf("The number of rows are: %s.\n", rows);
 	//printf("The number of columns are: %s.\n", columns);

 	if(isInteger(rows) == false || isInteger(columns) == false){
 		printf("You should use only integers for the size of the matrix in the given file.\n");
 		exit(0);
 	}

 	matrixPtr->numColumns = atoi(columns);//storing the number of columns for that matrix as int
 	matrixPtr->numRows = atoi(rows);//storing the number of rows for that matrix as int
 	matrixPtr->matrix = matrixInitializer(matrixPtr->numRows, matrixPtr->numColumns*2);////just initializing dynamically the array of array for saving the elements from the file for that matrix

 	while(fscanf(file, "%s", auxArray) != EOF){//until the file ends
 		if(count == matrixPtr->numColumns){
 			count = 0;//setting the count as 0 when the values from the file on the columns have finished
 			//printf("\n");
 			posColumns = 0;//setting the position for the columns to prevent overflow
 			posRows++;//if there are no more columns, continue to the second row and so on.
 		}
 		if (isFloat(auxArray) == true){//checjing if the value is an authentic float data type
 			matrixPtr->matrix[posRows][posColumns] = atof(auxArray);//setting the value from the file to the matrix
 			//printf("%0.2f ",matrixPtr->matrix[posRows][posColumns]);
 			count++;
 			posColumns++;
 		}
 		else{
 			printf("The file must contain only float data type. Resolve it and run it again.\n");
 			exit(0);
 		}
 		
	}
	//printf("\n\n");
 	free(rows);//free is for releasing the memory for the dynamically array
 	free(columns);
 	free(auxArray);
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
	int cont = 0;//just for knowing if the value have two or more '.'
	for (int i = 0; i < strlen(givenNumber); i++){
		if(!isdigit(givenNumber[i])){
			if (givenNumber[i] == '.' && cont == 0){
				cont++;
			}
			else{
				return false;
			}
	    }
	}
	return true;
}

float ** matrixInitializer(int numberOfRows, int numberOfColumns){
 	float ** matrix = (float **)calloc(numberOfRows*numberOfColumns, sizeof(float));//declaring the array of array dynamically
	for(int i = 0; i < numberOfRows; i++){
		matrix[i] = (float *)calloc(numberOfColumns, sizeof(float));//declaring the array dynamically
 	}
	return matrix;
}

float ** matrixMultiplication(struct Matrix * matrix1, struct Matrix * matrix2){
	if(checkCanMultiply(matrix1, matrix2) == false){
		printf("The given matrices can not be multiplied.\n");
		exit(0);
	}
 	float ** result = matrixInitializer(matrix1->numRows, matrix2->numColumns);

	for(int i = 0; i < matrix1->numRows; i++){//go over the rows for the first matrix
        for(int j = 0; j < matrix2->numColumns; j++){//go over the columns for the second matrix
            for(int k = 0; k < matrix1->numColumns; k++){//go over the columns for the first matrix
                result[i][j] += matrix1->matrix[i][k] * matrix2->matrix[k][j];//for stores the sum for each row/column
            }
        }
	}
	releaseMemory(matrix1->matrix, matrix1->numRows);//for release the assigned memory 
	releaseMemory(matrix2->matrix, matrix2->numRows);
	return result;
}

bool checkCanMultiply(struct Matrix * matrix1, struct Matrix * matrix2){
	if(matrix1->numColumns == matrix2->numRows)
	{
		return true;
	}
	else
		return false;
}

void printInTheGivenFile(char * fileName, float ** result, int numRows, int numColumns){
	FILE *file = NULL;
	file = openFile(fileName, "w");//w = write
	for (int i = 0; i < numRows; i++){
		for (int j = 0; j < numColumns; j++){
			fprintf(file, "%0.2f ", result[i][j]);//for print in the file
		}
		fprintf(file, "\n");
	}
	fclose(file);
	//free(result);
	releaseMemory(result, numRows);
}

void releaseMemory(float ** matrix, int numberOfRows){
	for (int i = 0; i < numberOfRows; i++){
		free(matrix[i]);//free the assigned memory for the array in each slot from the father array
	}
	free(matrix);//free the memory for the array of arrays
}
