/*
Alexandro Francisco Marcelo Gonzalez A01021383
*/
#include "image_tools.h"

int main(int argc, char *argv[])
{
	int c;
	char * input_file;
	char * output_file;
	char * sChar;
	struct Image img;
	int iAux = 0;
	int oAux = 0;
	int nAux = 0;
	int sAux = 0;
	int scaleFactor;

	while((c = getopt(argc, argv, "i:o:ns:")) != -1){
		switch(c){
			case 'i':
				iAux++;
				input_file = optarg;
				break;
			case 'o':
				oAux++;
				output_file = optarg;
				break;
			case 'n':
				nAux++;
				break;
			case 's':
				sAux++;
				printf("scale");
				sChar = optarg;
				scaleFactor = atoi(sChar);
				break;
		}
	}
	
	if (iAux > 0 && oAux > 0){//just to ensure the input and output file is given
		img.fileName = input_file;
		readPPMAndStore(img.fileName, &img);//calling the function for reading the data from the file
		if (nAux > 0) {
			negativeImage(&img);//converting to negative
		}
		if (sAux > 0) {
			if(scaleFactor <= 100){//for down scale
				scaleImageDown(&img, scaleFactor);//scale the image 
			}
			else if(scaleFactor > 100){//for up scale
				scaleImageUp(&img, scaleFactor);
			}
		}
		writeImage(output_file, &img);//writting the data into the file
		releaseMemory(&img);//releasing the memory
	}
	else{
		printf("Make sure that you give me the input and output file.\n");
	}
	
	return 0;
}
