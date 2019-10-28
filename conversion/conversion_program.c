/*
Alexandro Francisco Marcelo Gonzalez A01021383
Advanced Programming
january 24, 2019
Program for converting from N base number to N base using arguments while compiling
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#define MAX 100 //for creating the static array for saving the converted number each time it leaves each function fro prevent loosing the content

char * toDecimal(char givenValue[], int inputBase, int outputBase);
char * fromDecimal(int decimalValue, int base);
char * changePositionsConvertedNumber(int length, char convertedToGivenBase[]);
bool checkTheGivenBase(char givenBase[]);

int main(int argc, char * argv[])
{
	char convertedNumber[MAX][MAX];//saving each outcome for the given numbers to be converted
	char * input = NULL;//save the -i argument
	char * output = NULL;//save the -o argument
	int inputBase, outputBase;//to cast to input and output arguments
	int c;
	int numberOfArguments = 0;
	while ((c = getopt (argc, argv, "i:o:")) != -1){//to obtain the arguments in terminal. Source: https://pubs.opengroup.org/onlinepubs/009696799/functions/getopt.html
	    switch (c)
    	{
    		case 'i':
        		//printf("The input base value is: %s\n", argv[2]);
        		input = optarg;
        		if(checkTheGivenBase(input)){//if the given input is not a base, exit the program
        			printf("The input base (-i) is not correct\n");
        			return -1;
				}
				inputBase = atoi(input);//cast the given base
        		break;
      		case 'o':
        		//printf("The output base value is: %s\n", argv[4]);
        		output = optarg;
        		if(checkTheGivenBase(output)){//if the given input is not a base, then...
        			printf("The output base (-o) is not correct\n");
        			return -1;
				}
       			outputBase = atoi(output);
        		break;
	      	default:
	        	printf("Arguments have to be correct. Run it again\n");
	        	return 0;
	    }
	}
	//printf("Input value= %s\n", input);
    //printf("Output value= %s\n", output);
	if (!input || !output)//just to verify that the user puts the -i and -o arguments correctly
	{
		printf("Missing argument(s). Run it again.\n");
		return 0;
	}
	printf("\n");
    for (int i = optind; i < argc; ++i)
    {
    	printf("Converting %s from base %s to base %s\n", argv[i], input, output);
    	strcpy(convertedNumber[numberOfArguments],toDecimal(argv[i], inputBase, outputBase));//i need to use trcpy tu use the outcome for each given value
    	printf("Converted to base %s: %s\n",output, convertedNumber[numberOfArguments]);
    	printf("\n");
    	numberOfArguments++;
    }
	return 0;
}//main

//FUNCTIONS
bool checkTheGivenBase(char givenBase[]){//just to check if the given input/output base is a number
	int ln = strlen(givenBase);
	for(int i = 0; i < ln; i++){
	    if(!isdigit(givenBase[i])){
	        return true;
	    }
	}
	return false;
}//checkTheGivenBase

char * fromDecimal(int decimalValue, int outputBase){//transform always from decimal to the given output base
	int j = 0;
	char convertedToNBase[MAX];
	static char convertedNumber[MAX];//for save the returned char from other function
	memset(convertedNumber, 0, sizeof convertedNumber);//just for cleaning the static array used for saving the converted number, otherwise the array is overwritten each given argument
	while(decimalValue>0){
		int remainder = decimalValue % outputBase;
		if (remainder<10)
		{
			convertedToNBase[j] = 48 + remainder; //because is in ascii code, so the 48 represents the 0, and adding the remainder gets the decimal number
		}
		else{
			convertedToNBase[j] = 55 + remainder; //this because 55 represents the letters (55='A'), when adding the reminder gets the corresponding letter for the given base
		}
		decimalValue = decimalValue / outputBase;
		j++;
	}
	strcpy(convertedNumber, changePositionsConvertedNumber(j, convertedToNBase));
	return convertedNumber;
}//fromDecimal

char * toDecimal(char givenValue[], int inputBase, int outputBase){//transform always the given number to decimal, because its easier
	static char convertedNumber[MAX];
	memset(convertedNumber, 0, sizeof convertedNumber);//just for cleaning the static array used for saving the converted number, otherwise the array is overwritten each given argument
	int tamGivenValue = strlen(givenValue);
	int exponent = tamGivenValue-1;
	long int outcome = 0;
	int asciiCode;//to obtain the real value for the ascii code
	//printf("Input Base is =  %d\n",inputBase);
	//printf("The tam is: %d\n", tamGivenValue);
	for (int i = 0; i < tamGivenValue; i++, exponent--)
	{
		if (givenValue[i] >= 48 && givenValue[i] <= 57)//in ascii code: if it is a number
		{
			asciiCode = givenValue[i] - 48;
		}
		else if (givenValue[i] >= 65 && givenValue[i] <= 90)//in ascii code: if it is a uppercase letter
		{
			asciiCode = givenValue[i] - 55;
		}
		else if (givenValue[i] >= 97 && givenValue[i] <= 122){//in ascii code: if it is a lowercase letter
			printf("Only accepts uppercase letters, please run the program again correctly\n");
			exit(0);
		}
		else{//anything else
			printf("Found special character, please run the program again correctly\n");
			exit(0);
		}
		//printf("Ascii code of %c= %d.\n",givenValue[i],givenValue[i]);
		outcome = outcome + (asciiCode * pow(inputBase,exponent));
	}
	//printf("Outcome in decimal base = %ld\n", outcome);
	strcpy(convertedNumber,fromDecimal(outcome, outputBase));
	return convertedNumber;
}

char * changePositionsConvertedNumber(int length, char convertedToGivenBase[]){//used for reorganized correctly the converted number because the function fromDecimal brings the converted number from end to the beginning
	static char convertedNumber[MAX];
	memset(convertedNumber, 0, sizeof convertedNumber);//just for cleaning the static array used for saving the converted number, otherwise the array is overwritten each given argument
	int j = 0;
	for (int i = length-1; i >= 0; i--, j++){
		//printf("%c", convertedToGivenBase[i]);
		convertedNumber[j] = convertedToGivenBase[i];
	}
	return convertedNumber;//return the correct converted number
}
