/*
Alexandro Francisco Marcelo Gonzalez A01021383
*/
#ifndef IMAGE_TOOLS_H
#define IMAGE_TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

struct ImagePixel{//creating a struct for storing the rgb data for each pixel
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

struct Image{//for storing the entire image with a ImagePixel struct inside 
    char * fileName;
    char * magicNumber;
    int width;
    int height;
    int maxValues;
    struct ImagePixel * data;
};

struct Filter_Image{//for storing the filter matrix
    char * fileName;
    float ** matrix;
    int width;
    int height;
};

FILE * openFile(char * fileName, char * mode);
void readPPMAndStore(char * fileName, struct Image * img);
void readPPMAndStoreAscii(FILE * file, struct Image * img);
void readPPMAndStoreBinary(FILE * file, struct Image * img);
void negativeImage(struct Image * img);
void scaleImageUp(struct Image * img, int scaleFactor);
void scaleImageDown(struct Image * img, int scaleFactor);
void printImage(struct Image * img);
void writeImage(char * fileName, struct Image * img);
void writeImageAscii(FILE * file, struct Image * img);
void writeImageBinary(FILE * file, struct Image * img);
int checkBinaryOrAscii(FILE * file, struct Image * img);
void releaseMemory(struct Image * img);

void readFilterImage(char * fileName, struct Filter_Image * filter);
void filterTheImage(struct Image * original_image, struct Filter_Image * filter);

#endif
