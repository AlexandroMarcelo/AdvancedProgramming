/*
Alexandro Francisco Marcelo Gonzalez A01021383
*/
#include "image_tools.h"

FILE * openFile(char * fileName, char * mode){
	FILE * file = NULL;
	file = fopen (fileName, mode);//opening and reading/writing the given file
	if (file == NULL)	{
		printf("Error while opening the file. Make sure you pass correctly the name through arguments while compiling.\n");
		exit(0);
	}
	return file;
}

int checkBinaryOrAscii(FILE * file, struct Image * img){
	if (img->magicNumber[1] == '3') {//if the magic number is P3: ascii
		return 3;
	}
	else if(img->magicNumber[1] == '6'){//if the magic number is P6: binary
		return 6;
	}
	else {
		printf("I cant recognize the magic number\n ");
		exit(0);
	}
}

void readPPMAndStore(char * fileName, struct Image * img){
	FILE * file = NULL;
	file = openFile(fileName, "rb"); 
	img->magicNumber = (char *)malloc(sizeof(char));//allocating memory for the magic number

	fscanf(file, "%s", img->magicNumber);//storing the magic number 'P6'...
	//printf("Magic Number= %s\n", img->magicNumber);//for the magic number
	if (checkBinaryOrAscii(file, img) == 3){
		readPPMAndStoreAscii(file, img);//calling the function to read and store in ascii 
	}
	else {
		readPPMAndStoreBinary(file, img);//calling the function to read and store in binary 
	}
	fclose(file);//closing the file
}

void readPPMAndStoreAscii(FILE * file, struct Image * img){
	char * comments = (char *)malloc(sizeof(char));//allocating memory for comments if there are 

	int c = getc(file);//eliminate the '\n'
	c = getc(file);//get the next char just in case of comments with #
	if (c=='#'){//for eliminate the comments in case of it
		fgets(comments, 1000, file);//eliminate the comments in the file
		//printf("Comments= %s\n", comments);
	}
	else{
		ungetc(c,file);//undo getting the char for the comment
	}
	
	fscanf(file, "%d", &img->width);//storing the width
	//printf("Width = %d\n", img->width);
	fscanf(file, "%d", &img->height);//store the height
	//printf("Height = %d\n", img->height);
	fscanf(file, "%d", &img->maxValues);//storing the maximun values
	//printf("MaxValues = %d\n", img->maxValues);

	getc(file);//eliminate the '\n'

    //allocating memory for the data
    img->data = (struct ImagePixel *)calloc(img->width * img->height, sizeof(struct ImagePixel));
	for(int i = 0; i < img->width*img->height; i++){
		fscanf(file, "%hhu", &img->data[i].red);
		fscanf(file, "%hhu", &img->data[i].green);
		fscanf(file, "%hhu", &img->data[i].blue);
	}	
    free(comments);
}

void readPPMAndStoreBinary(FILE * file, struct Image * img){
	char * comments = (char *)malloc(sizeof(char));//allocating memory for comments if there are 

	int c = getc(file);//eliminate the '\n'
	c = getc(file);//get the next char just in case of comments with #
	if (c=='#'){//for eliminate the comments in case of it
		fgets(comments, 1000, file);//eliminate the comments in the file
		//printf("Comments= %s\n", comments);
	}
	else{
		ungetc(c,file);//undo getting the char for the comment
	}
	
	fscanf(file, "%d", &img->width);//storing the width
	//printf("Width B= %d\n", img->width);
	fscanf(file, "%d", &img->height);//store the height
	//printf("Height  B= %d\n", img->height);
	fscanf(file, "%d", &img->maxValues);//storing the maximun values
	//printf("MaxValues  B= %d\n", img->maxValues);

	getc(file);//eliminate the '\n'
    //allocating memory for the data
    img->data = (struct ImagePixel *)calloc(img->width * img->height, sizeof(struct ImagePixel));
	fread(img->data, 3 * img->width, img->height, file);//reading each pixel from given file
    free(comments);
	fclose(file);//closing the file
}

void negativeImage(struct Image * img){//transform the image to negative
	int numPixels = img->width*img->height;
	for(int i = 0; i < numPixels; i++){
		//storing the new negative value for each RGB data
		img->data[i].red = img->maxValues-img->data[i].red;
		img->data[i].green = img->maxValues-img->data[i].green;
		img->data[i].blue = img->maxValues-img->data[i].blue;
	}
}

void printImage(struct Image * img){//Just for debugging
	int numPixels = img->width*img->height;
	printf("Num PIxels = %d\n", numPixels);
	for(int i = 0; i < numPixels; i++)
	{
		printf("%hhu ", img->data[i].red);
		printf("%hhu ", img->data[i].green);
		printf("%hhu   ", img->data[i].blue);
	}
	printf("\n");
}

void writeImage(char * fileName, struct Image * img){
	FILE * file = NULL;
	file = openFile(fileName, "wb"); 
	
	if (checkBinaryOrAscii(file, img) == 3){
		writeImageAscii(file, img);
	}
	else {
		writeImageBinary(file, img);
	}
}

void writeImageBinary(FILE * file, struct Image * img){//writting in the file with the new transformed image
    
	fprintf(file, "%s\n", img->magicNumber);//printing the magic number for the image
    fprintf(file, "%d %d\n",img->width,img->height);//printing the width and height
    fprintf(file, "%d\n",img->maxValues);//printing the maximum values

    fwrite(img->data, 3 * img->width, img->height, file);//writting the data in the file
    fclose(file);//closing the writed file
}

void writeImageAscii(FILE * file, struct Image * img){//writting in the file with the new transformed image
    fprintf(file, "%s\n", img->magicNumber);//printing the magic number for the image
    fprintf(file, "%d %d\n",img->width,img->height);//printing the width and height
    fprintf(file, "%d\n",img->maxValues);//printing the maximum values

	for(int i = 0; i < img->width*img->height; i++){
		fprintf(file, "%hhu ", img->data[i].red);
		fprintf(file, "%hhu ", img->data[i].green);
		fprintf(file, "%hhu  ", img->data[i].blue);
	}
    fclose(file);//closing the writed file
}

void scaleImageUp(struct Image * img, int scaleFactor){
	struct Image imgAux;//creating a copy from the original image
	imgAux.fileName = img->fileName;
	readPPMAndStore(imgAux.fileName, &imgAux);
	scaleFactor = scaleFactor / 100;//because is percentage
	int newWidth = img->width*scaleFactor;//giving the new image width
	int newHeight = img->height*scaleFactor;//giving the new image height
	int jump = newWidth;//for jumping to the next line for the rest of the neighbors
	img->width = newWidth;//the new width of the image
	img->height = newHeight;//the new height of the image
	//img->data = (struct ImagePixel *) realloc(img->data, newWidth*newHeight);//reallocating the memory for the new data
	
	//variables for help while scaling
	int aux;
	int widthAux = 0;
	int cont = 0;
	for(int  s = 0; s < imgAux.height; s++){
		widthAux += (jump * s * scaleFactor);//to give the jump to the next pixel with its own neighbors

		for(int i = 0; i < imgAux.width; i++){
			aux = (i*scaleFactor) + widthAux;

			for(int j = 0; j < scaleFactor; j++){
				for(int k = 0; k < scaleFactor; k++){
					//creating the new pixel data
					aux+=k;
					img->data[aux].red = imgAux.data[cont].red;
					img->data[aux].green = imgAux.data[cont].green;
					img->data[aux].blue = imgAux.data[cont].blue;
				}
				aux+=jump-scaleFactor+1;
			}
			//printf("\ncont= %d ",cont);
			cont++;
		}
	}
	releaseMemory(&imgAux);
}

void scaleImageDown(struct Image * img, int scaleFactor){
	struct Image imgAux;//creating a copy from the original image
	imgAux.fileName = img->fileName;
	readPPMAndStore(imgAux.fileName, &imgAux);
	float newWidth = (float)img->width*((float)scaleFactor/100);//giving the new image width
	float newHeight = (float)img->height*((float)scaleFactor/100);//giving the new image height
	scaleFactor =  100/scaleFactor;//for take the neighbors of each pixel
	
	int jump = img->width;//for jumping to the next line for the rest of the neighbors
	img->width = (int) newWidth;//the new width of the image
	img->height = (int) newHeight;//the new height of the image

	//img->data = (struct ImagePixel *) calloc(newWidth*newHeight, sizeof(struct ImagePixel));//reallocating the memory for the new data
	//variables for help while scaling
	int aux;
	int widthAux = 0;
	int cont = 0;
	for(int  s = 0; s < imgAux.height; s++){
		for(int i = 0; i < imgAux.width; i++){
			aux = (i*scaleFactor) + widthAux;//to give the jump to the next pixel with its own neighbors

			for(int j = 0; j < scaleFactor; j++){
				aux+=j;
				//creating the new pixel 
				img->data[cont].red += (imgAux.data[aux].red + imgAux.data[aux+1].red);
				img->data[cont].green += (imgAux.data[aux].green + imgAux.data[aux+1].green);
				img->data[cont].blue += (imgAux.data[aux].blue + imgAux.data[aux+1].green);
				aux+=jump-scaleFactor+1;	
			}
			//average
			img->data[cont].red = img->data[cont].red/4;
			img->data[cont].green = img->data[cont].green/4;
			img->data[cont].blue = img->data[cont].blue/4;
		}
		//printf("\ncont= %d y red ",cont);
		cont++;
	}
	releaseMemory(&imgAux);
}

void releaseMemory(struct Image * img){//releasing the memory
	free(img->magicNumber);
	free(img->data);
}

void readFilterImage(char * fileName, struct Filter_Image * filter)
{
	FILE * file = NULL;
	file = openFile(fileName, "rb"); 
	
	fscanf(file, "%d", &filter->height);
	fscanf(file, "%d", &filter->width);

	filter->matrix = (float **)malloc(filter->height * sizeof(float *));

	for (int i = 0; i < filter->height; i++)
	{
		filter->matrix[i] = (float *)malloc(filter->width * sizeof(float));
	}
	for (int i = 0; i < filter->height; i++)
	{
		for (int j = 0; j < filter->width; j++)
		{
			fscanf(file, "%f", &filter->matrix[i][j]);
		}
	}
	
	for (int i = 0; i < filter->height; i++)
	{
		for (int j = 0; j < filter->width; j++)
		{
			printf("%f ", filter->matrix[i][j]);
		}
		printf("\n");
	}
	fclose(file);//closing the file
}

//filtering the given image with the given filter
void filterTheImage(struct Image * original_image, struct Filter_Image * filter){
	struct Image filtered_image;//creating a copy from the original image
	filtered_image.fileName = original_image->fileName;
	readPPMAndStore(filtered_image.fileName, &filtered_image);

	int jump = filter->height / 2;

	for (int i = jump; i < original_image->height-jump; i++)
	{
		for (int j = jump; j < original_image->width-jump; j++)
		{
			for (int k = i-jump; k < filter->height; k++)
			{
				for (int p = j-jump; p < filter->width; p++)
				{
					//original_image->data[k].red;
				}
				
			}
			
		}
	}
	
}
