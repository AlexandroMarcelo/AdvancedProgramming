/*
    Alexandro Francisco Marcelo Gonzalez A01021383
    25/04/2019

    Based in Gilberto Echeverria`s code
*/

#ifndef PPMS_H
#define PPMS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define NUM_THREADS 8

// Assign constants to the colors
typedef enum color_enum {R, G, B} color_t;

// Structure to store a single pixel
typedef struct pixel_struct {
    // One byte: Allow max value to be 255
    unsigned char data[3];
    // Two bytes: Allow max value to be 65535
    //unsigned short data[3];
} pixel_t;

// Structure for the PPM image
typedef struct ppm_struct {
    char magic_number[3];
    int width;
    int height;
    int max_value;
    pixel_t ** pixels;
} ppm_t;

//Structure for the filter
typedef struct Filter_Image{//for storing the filter matrix
    char * fileName;
    float ** matrix;
    int width;
    int height;
} filter_image_t;

//thread data
typedef struct thread_data_struct {
    int id;
    int distance;
    int start_height;
    int end_height;
    ppm_t * original_image;
    ppm_t filtered_image;
    filter_image_t * filter;
} thread_data_t;

///// FUNCTION DECLARATIONS

// Get the memory necessary to store an image of the size indicated in the structure
void allocateMemory(ppm_t * image);

// Release the memory for the image
void freeMemory(ppm_t * image);

// Open an image file and copy its contents to the structure
void readImageFile(char * filename, ppm_t * image, int padding);

// Write the data stored in a structure into an image file
void writeImageFile(char * filename, const ppm_t * image);

// Read each of the numerical values for the color components
//  and store them in the arrray for the pixels
void getAsciiPixels(ppm_t * image, FILE * file_ptr, int padding);

// Copy the bytes from the image into the data array for the pixels
void getBinaryPixels(ppm_t * image, FILE * file_ptr, int padding);

// Write the numerical values for the color components into
//  the output file, giving them a nice format
void writeAsciiPixels(const ppm_t * image, FILE * file_ptr);

// Copy the pixel data into the file as bytes
void writeBinaryPixels(const ppm_t * image, FILE * file_ptr);

// Compute the negative colors of an image
void getNegativeImage(ppm_t * image);

// Print the pixels of the image as hexadecimal on the console
void printPixels(ppm_t * image);

// Rotate the image a specified number of degrees
// Modify the structure received as a parameter
void rotateImage(ppm_t * source, int angle);

// Rotate the image and write the result in another structure
void rotateImage90(ppm_t * destination, const ppm_t * source);

// Rotate the image and write the result in another structure
void rotateImage180(ppm_t * destination, const ppm_t * source);

// Rotate the image and write the result in another structure
void rotateImage270(ppm_t * destination, const ppm_t * source);

// Scale an image by a given percentage
// Modify the structure received as a parameter
void scaleImage(ppm_t * source, int scale);

// Apply the scale to the pixels in an image
void resizeImage(ppm_t * destination, const ppm_t * source, int scale);

//filtering the given image with the given filter
void filterTheImage(ppm_t * original_image, filter_image_t * filter);

//Read the file with the filter and strore it in the structure
void readFilterImage(char * fileName, filter_image_t * filter);

//Free memory allocated for the filter file
void freeFilter(filter_image_t * filter);

//Make the duplicate of the structure to preserve the data
void copyStructureData(ppm_t * original_image, ppm_t * filtered_image, int start_height, int end_height);

//For each created thread, thread function
void * filterTheImageWithThreads(void * arg);

#endif  /* NOT PPMS_H */
