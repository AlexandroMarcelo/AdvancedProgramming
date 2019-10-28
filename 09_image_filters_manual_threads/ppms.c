/*
    Alexandro Francisco Marcelo Gonzalez A01021383
    25/04/2019

    Based in Gilberto Echeverria`s code
*/
#include "ppms.h"

///// FUNCTION DEFINITIONS

// Get the memory necessary to store an image of the size indicated in the structure
void allocateMemory(ppm_t * image)
{
    // Allocate the memory for INDEX array
    image->pixels = malloc (image->height * sizeof(pixel_t *));
    // Allocate the memory for all the DATA array
    image->pixels[0] = calloc (image->height * image->width, sizeof(pixel_t));
    
    // Add the rest of the pointers to the INDEX array
    for (int i=1; i<image->height; i++)
    {
        // Add an offset from the beginning of the DATA array
        image->pixels[i] = image->pixels[0] + image->width * i;
    }
}

// Release the memory for the image
void freeMemory(ppm_t * image)
{
    // Free the DATA array
    free (image->pixels[0]);
    // Free the INDEX array
    free (image->pixels);

    // Set the values for an empty image
    image->width = 0;
    image->height = 0;
    image->pixels = NULL;
}

// Open an image file and copy its contents to the structure
void readImageFile(char * filename, ppm_t * image, int padding)
{
    FILE * file_ptr = NULL;
    
    file_ptr = fopen (filename, "r");
    if (!file_ptr)
    {
        printf("Unable to open the file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
    
    // Get the data from the header
    fscanf (file_ptr, "%s", image->magic_number);
    fscanf (file_ptr, "%d", &image->width);
    fscanf (file_ptr, "%d", &image->height);
    fscanf (file_ptr, "%d", &image->max_value);
    // Get rid of the '\n' after max value and before the real data
    image->height += padding;
    //image->width += padding;
    fgetc(file_ptr);

    // Get the memory for the image data
    allocateMemory(image);

    for (int r = 0; r < padding; r++)
    {
        for (int c = 0; c < padding; c++)
        {
            image->pixels[r][c].data[R] = 0;
            image->pixels[r][c].data[G] = 0;
            image->pixels[r][c].data[B] = 0;
        }
    }
    

    if (!strncmp(image->magic_number, "P3", 3))
    {
        getAsciiPixels(image, file_ptr, padding);
    }
    else if (!strncmp(image->magic_number, "P6", 3))
    {
        getBinaryPixels(image, file_ptr, padding);
    }
    else
    {
        printf("Not a correct PPM format");
    }

    fclose(file_ptr);
}

// Write the data stored in a structure into an image file
void writeImageFile(char * filename, const ppm_t * image)
{
    FILE * file_ptr = NULL;
    
    file_ptr = fopen (filename, "w");
    if (!file_ptr)
    {
        printf("Unable to open the file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
    
    // Insert the header
    fprintf (file_ptr, "%s\n", image->magic_number);
    fprintf (file_ptr, "%d ", image->width);
    fprintf (file_ptr, "%d\n", image->height);
    fprintf (file_ptr, "%d\n", image->max_value);

    // Write the matrix of data for the pixels
    if (!strncmp(image->magic_number, "P3", 3))
    {
        writeAsciiPixels(image, file_ptr);
    }
    else if (!strncmp(image->magic_number, "P6", 3))
    {
        writeBinaryPixels(image, file_ptr);
    }
    else
    {
        printf("Not a correct PPM format");
    }

    fclose(file_ptr);
}

// Read each of the numerical values for the color components
//  and store them in the arrray for the pixels
void getAsciiPixels(ppm_t * image, FILE * file_ptr, int padding)
{
    for (int r=padding; r<image->height; r++)
    {
        for (int c=padding; c<image->width; c++)
        {
            fscanf (file_ptr, "%hhu", &image->pixels[r][c].data[R]);
            fscanf (file_ptr, "%hhu", &image->pixels[r][c].data[G]);
            fscanf (file_ptr, "%hhu", &image->pixels[r][c].data[B]);
            //fscanf (file_ptr, "%hu", &image->pixels[r][c].data[B]);
        }
    }
}

// Copy the bytes from the image into the data array for the pixels
void getBinaryPixels(ppm_t * image, FILE * file_ptr, int padding)
{
    fread (image->pixels[padding], sizeof(pixel_t), image->width * image->height, file_ptr);    
}

// Write the numerical values for the color components into
//  the output file, giving them a nice format
void writeAsciiPixels(const ppm_t * image, FILE * file_ptr)
{
    for (int r=0; r<image->height; r++)
    {
        for (int c=0; c<image->width; c++)
        {
            fprintf (file_ptr, "%3hhu ", image->pixels[r][c].data[R]);
            fprintf (file_ptr, "%3hhu ", image->pixels[r][c].data[G]);
            fprintf (file_ptr, "%3hhu\t", image->pixels[r][c].data[B]);
        }
        fprintf(file_ptr, "\n");
    }
}

// Copy the pixel data into the file as bytes
void writeBinaryPixels(const ppm_t * image, FILE * file_ptr)
{
    fwrite (image->pixels[0], sizeof(pixel_t), image->width * image->height, file_ptr);    
}

// Compute the negative colors of an image
void getNegativeImage(ppm_t * image)
{
    for (int r=0; r<image->height; r++)
    {
        for (int c=0; c<image->width; c++)
        {
            // Loop for the 3 color components
            for (int color=0; color<3; color++)
            {
                image->pixels[r][c].data[color] = image->max_value - image->pixels[r][c].data[color];
            }
        }
    }
}

// Print the pixels of the image as hexadecimal on the console
void printPixels(ppm_t * image)
{
    for (int r=0; r<image->height; r++)
    {
        for (int c=0; c<image->width; c++)
        {
            // Loop for the 3 color components
            for (int color=0; color<3; color++)
            {
                printf("%02x ", image->pixels[r][c].data[color]);
            }
            printf("\t");
        }
        printf("\n");
    }
}


// Rotate the image a specified number of degrees
// Modify the structure received as a parameter
void rotateImage(ppm_t * source, int angle)
{
    // Local variable for an image structure
    ppm_t destination = {"", 0, 0, 0, NULL};

    // Copy the header data
    strncpy (destination.magic_number, source->magic_number, 3);
    destination.max_value = source->max_value;

    switch (angle)
    {
        case 90:
            // Invert the sizes
            destination.height = source->width;
            destination.width = source->height;
            // Get the memory for the image data
            allocateMemory(&destination);
            // Apply the rotation
            rotateImage90(&destination, source);
            break;
        case 180:
            // Keep the same image ration
            destination.height = source->height;
            destination.width = source->width;
            // Get the memory for the image data
            allocateMemory(&destination);
            // Apply the rotation
            rotateImage180(&destination, source);
            break;
        case 270:
            // Invert the sizes
            destination.height = source->width;
            destination.width = source->height;
            // Get the memory for the image data
            allocateMemory(&destination);
            // Apply the rotation
            rotateImage270(&destination, source);
            break;
        default:
            printf("Can only rotate the image 90, 180 or 270 degrees. Sorry!\n");
            break;
    }

    // Free the previous memory data
    freeMemory(source);
    // Copy the results back to the pointer received
    *source = destination;
}

// Rotate the image and write the result in another structure
void rotateImage90(ppm_t * destination, const ppm_t * source)
{
    // Prepare the limits for the loops
    int max_row = destination->height - 1;

    // Loops to reallocate the pixels
    for (int row_src=0, col_dst=0; row_src<source->height; row_src++, col_dst++)
    {
        for (int col_src=0, row_dst=max_row; col_src<source->width; col_src++, row_dst--)
        {
            // Loop for the 3 color components
            for (int color=0; color<3; color++)
            {
                destination->pixels[row_dst][col_dst].data[color] = source->pixels[row_src][col_src].data[color];
            }
        }
    }
}

// Rotate the image and write the result in another structure
void rotateImage180(ppm_t * destination, const ppm_t * source)
{
    // Prepare the limits for the loops
    int max_row = destination->height - 1;
    int max_col = destination->width - 1;

    // Loops to reallocate the pixels
    for (int row_src=0, row_dst=max_row; row_src<source->height; row_src++, row_dst--)
    {
        for (int col_src=0, col_dst=max_col; col_src<source->width; col_src++, col_dst--)
        {
            // Loop for the 3 color components
            for (int color=0; color<3; color++)
            {
                destination->pixels[row_dst][col_dst].data[color] = source->pixels[row_src][col_src].data[color];
            }
        }
    }
}

// Rotate the image and write the result in another structure
void rotateImage270(ppm_t * destination, const ppm_t * source)
{
    // Prepare the limits for the loops
    int max_col = destination->width - 1;

    // Loops to reallocate the pixels
    for (int row_src=0, col_dst=max_col; row_src<source->height; row_src++, col_dst--)
    {
        for (int col_src=0, row_dst=0; col_src<source->width; col_src++, row_dst++)
        {
            // Loop for the 3 color components
            for (int color=0; color<3; color++)
            {
                destination->pixels[row_dst][col_dst].data[color] = source->pixels[row_src][col_src].data[color];
            }
        }
    }
}

// Scale an image by a given percentage
// Modify the structure received as a parameter
void scaleImage(ppm_t * source, int scale)
{
    // Local variable for an image structure
    ppm_t destination = {"", 0, 0, 0, NULL};

    // Copy the header data
    strncpy (destination.magic_number, source->magic_number, 3);
    destination.max_value = source->max_value;

    // Keep the same image ratio
    destination.height = source->height * (scale / 100.0);
    destination.width = source->width * (scale / 100.0);

    printf("New width: %d, height: %d\n", destination.width, destination.height);
    // Get the memory for the image data
    allocateMemory(&destination);
    // Apply the scale
    resizeImage(&destination, source, scale);
   
    // Free the previous memory data
    freeMemory(source);
    // Copy the results back to the pointer received
    *source = destination;
}

// Apply the scale to the pixels in an image
void resizeImage(ppm_t * destination, const ppm_t * source, int scale)
{
    // Compute the multiplication factors depending on the relative sizes
    double factor_x = source->width / (double) destination->width;
    double factor_y = source->height / (double) destination->height;
    int sum[3];
    // The sum of the pixels will be multiplied by this constant to get the average
    double divisor = 1 / (factor_x * factor_y);

    // Average the colors only when scaling down
    if (divisor > 1)
    {
        divisor = 1;
    }

#ifdef DEBUG
    printf("Factor_X: %lf | Factor_Y: %lf | Multiplied: %lf\n", factor_x, factor_y, factor_x * factor_y);
    printf("1 / Factor_X: %lf | Factor_Y: %lf | Multiplied: %lf\n", 1/factor_x, 1/factor_y, 1/(factor_x * factor_y));
#endif

    // Loops to fill in the pixels in the resized image
    for (int row_dst=0; row_dst<destination->height; row_dst++)
    {
        for (int col_dst=0; col_dst<destination->width; col_dst++)
        {
#ifdef DEBUG
            printf("DEBUG: Generating pixel %d, %d\n", row_dst, col_dst);
#endif
            // Reset the values for the new pixel
            for (int color=0; color<3; color++)
            {
                sum[color] = 0;
            }
#ifdef DEBUG
            printf("DEBUG: \tCollapsing pixel rows %lf, %lf\n", row_dst*factor_y, (row_dst+1)*factor_y);
#endif
            // Loops to average the pixels in the original image
            for (int row_src=row_dst*factor_y; row_src<(row_dst+1)*factor_y; row_src++)
            {
#ifdef DEBUG
            printf("DEBUG: \tCollapsing pixel columns %lf, %lf\n", col_dst*factor_x, (col_dst+1)*factor_x);
#endif
                for (int col_src=col_dst*factor_x; col_src<(col_dst+1)*factor_x; col_src++)
                {
#ifdef DEBUG
            printf("DEBUG: \tCollapsing source pixel %d, %d\n", row_src, col_src);
#endif
                    // Loop for the 3 color components
                    for (int color=0; color<3; color++)
                    {
                        sum[color] += source->pixels[row_src][col_src].data[color];
                    }
                }
            }

#ifdef DEBUG
            printf("DEBUG: \tsum: %d, %d, %d\n", sum[0], sum[1], sum[2]);
#endif
            // Assign the new color to the destination pixel
            for (int color=0; color<3; color++)
            {
                destination->pixels[row_dst][col_dst].data[color] = sum[color] * divisor;
            }

        }
    }
}

//Read the file with the filter and strore it in the structure
void readFilterImage(char * fileName, struct Filter_Image * filter)
{
	FILE * file = NULL;
	file = fopen(fileName, "rb");; 
	
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
	fclose(file);//closing the file
}

//Free memory allocated for the filter file
void freeFilter(filter_image_t * filter)
{
    for (int i = 0; i < filter->height; i++)
	{
		free(filter->matrix[i]);
	}
    free(filter->matrix);   
}

//filtering the given image with the given filter using manually created threads 
void filterTheImage(ppm_t * original_image, filter_image_t * filter)
{
    pthread_t tid[NUM_THREADS];
    int status;
    int height_jump = original_image->height/NUM_THREADS;
    int start_height_jump = 0;
    int end_height_jump = 0;
    thread_data_t dataThread[NUM_THREADS];
    int aux = 0;
    int distance = filter->height / 2;

    for (int i = 0; i < NUM_THREADS; i++)
    {
        start_height_jump = end_height_jump;
        end_height_jump += height_jump;
        ppm_t filtered_image;//creating a copy from the original image
        copyStructureData(original_image, &filtered_image, start_height_jump, end_height_jump);
        dataThread[i].id = i;
        dataThread[i].distance = distance;
        dataThread[i].start_height = start_height_jump;
        dataThread[i].end_height = end_height_jump;
        dataThread[i].original_image = original_image;
        dataThread[i].filtered_image = filtered_image;
        dataThread[i].filter = filter;

        status = pthread_create(&tid[i], NULL, &filterTheImageWithThreads, &dataThread[i]);
        if (status)
        {
            fprintf(stderr, "ERROR: pthread_create %d\n", status);
            exit(EXIT_FAILURE);
        }
        //printf("Started thread: %d\n", i);
    }
    
    // Wait for the threads to finish
    for (int i = 0; i < NUM_THREADS; i++)
    {
        status = pthread_join(tid[i], NULL);
        if (status)
        {
            fprintf(stderr, "ERROR: pthread_join %d\n", status);
            exit(EXIT_FAILURE);
        }
    }
}

//For each created thread, thread function
void * filterTheImageWithThreads(void * arg)
{
    thread_data_t * thread_data = (thread_data_t *)arg;
    double new_value_R;
    double new_value_G;
    double new_value_B;
    int cont = 0;
    int aux = 0;
    if (thread_data->id != 0)
    {
        aux = thread_data->start_height-thread_data->distance;
        printf("ID: %d\n", thread_data->id);
    }
    
//#pragma omp parallel for default(none) shared(distance, original_image->pixels) private(new_value_R, new_value_G, new_value_B, filtered_image)
	for (int rows_image = thread_data->start_height+thread_data->distance; rows_image < thread_data->end_height+thread_data->distance; rows_image++)
	{
		for (int columns_image = thread_data->distance; columns_image < thread_data->original_image->width-thread_data->distance; columns_image++)
		{
            new_value_R = 0;
            new_value_G = 0;
            new_value_B = 0;
			for (int neighbors_rows = rows_image-thread_data->distance; neighbors_rows < rows_image+thread_data->distance+1; neighbors_rows++) //make it always the size of the filter (height)
			{
				for (int neighbors_columns = columns_image-thread_data->distance; neighbors_columns < columns_image+thread_data->distance+1; neighbors_columns++) //make it always the size of the filter (width)
				{
					new_value_R += thread_data->original_image->pixels[neighbors_rows][neighbors_columns].data[R] * thread_data->filter->matrix[neighbors_rows+thread_data->distance-rows_image][neighbors_columns+thread_data->distance-columns_image];
                    new_value_G += thread_data->original_image->pixels[neighbors_rows][neighbors_columns].data[G] * thread_data->filter->matrix[neighbors_rows+thread_data->distance-rows_image][neighbors_columns+thread_data->distance-columns_image];
                    new_value_B += thread_data->original_image->pixels[neighbors_rows][neighbors_columns].data[B] * thread_data->filter->matrix[neighbors_rows+thread_data->distance-rows_image][neighbors_columns+thread_data->distance-columns_image];
                }
			}
            if (new_value_R > 255){
                new_value_R = 255;
            }
            if (new_value_G > 255){
                new_value_G = 255;
            }
            if (new_value_B > 255){
                new_value_B = 255;
            }
            if (new_value_R < 0){
                new_value_R = 0;
            }
            if (new_value_G < 0){
                new_value_G = 0;
            }
            if (new_value_B < 0){
                new_value_B = 0;
            }
            thread_data->filtered_image.pixels[cont][columns_image-thread_data->distance].data[R] = new_value_R;
            thread_data->filtered_image.pixels[cont][columns_image-thread_data->distance].data[G] = new_value_G;
            thread_data->filtered_image.pixels[cont][columns_image-thread_data->distance].data[B] = new_value_B;
		}
        cont++;
	}
    cont = 0;
    aux = 0;
    if (thread_data->id != 0)
    {
        aux = thread_data->start_height-thread_data->distance;
    }else
    {
        aux = 0;
    }
    printf("PP");
    
    for (int r = thread_data->start_height; r < thread_data->end_height; r++)
    {
        for (int c = thread_data->distance; c < thread_data->original_image->width-thread_data->distance; c++)
        {
            if (c > 0 && c < thread_data->original_image->width)
            {
                thread_data->original_image->pixels[r][c-thread_data->distance].data[R] = thread_data->filtered_image.pixels[cont][c].data[R];
                thread_data->original_image->pixels[r][c-thread_data->distance].data[G] = thread_data->filtered_image.pixels[cont][c].data[G];
                thread_data->original_image->pixels[r][c-thread_data->distance].data[B] = thread_data->filtered_image.pixels[cont][c].data[B];
            }
            
            
        }
        cont++;
    }
    
    // Free the previous memory data
    //freeMemory(original_image);
	//*original_image = filtered_image;
    //freeMemory(&filtered_image);
    pthread_exit(NULL);
}

//Make the duplicate of the structure to preserve the data
void copyStructureData(ppm_t * original_image, ppm_t * filtered_image, int start_height, int end_height)
{
    int cont = 0;
    filtered_image->height = end_height-start_height;
    filtered_image->width = original_image->width;
    strncpy (filtered_image->magic_number, original_image->magic_number, 3);
    filtered_image->max_value = original_image->max_value;
    allocateMemory(filtered_image);
    
    for (int r = start_height; r < end_height; r++)
    {
        for (int c = 0; c < original_image->width; c++)
        {
            filtered_image->pixels[cont][c].data[R] = original_image->pixels[r][c].data[R];
            filtered_image->pixels[cont][c].data[G] = original_image->pixels[r][c].data[G];
            filtered_image->pixels[cont][c].data[B] = original_image->pixels[r][c].data[B];
        }
        cont++;
    }
}

/*
for (int r = thread_data->start_height; r < thread_data->end_height; r++)
    {
        for (int c = 0; c < thread_data->original_image->width; c++)
        {
            if (r > 0 && r < thread_data->end_height)
            {
                //printf("[%d][%d]\n",r,c);
                thread_data->original_image->pixels[r][c].data[R] = thread_data->filtered_image.pixels[cont][c].data[R];
                thread_data->original_image->pixels[r][c].data[G] = thread_data->filtered_image.pixels[cont][c].data[G];
                thread_data->original_image->pixels[r][c].data[B] = thread_data->filtered_image.pixels[cont][c].data[B];
            }
            
            
        }
        cont++;
    }
*/

/*
//filtering the given image with the given filter
void filterTheImage(ppm_t * original_image, struct Filter_Image * filter)
{
	ppm_t filtered_image;//creating a copy from the original image
    copyStructureData(original_image, &filtered_image);
    double new_value_R;
    double new_value_G;
    double new_value_B;
	int distance = filter->height / 2;
    //int cont = 0;
    unsigned char hola;

//#pragma omp parallel for default(none) shared(distance, original_image->pixels) private(new_value_R, new_value_G, new_value_B, filtered_image)
	for (int rows_image = distance; rows_image < original_image->height-distance; rows_image++)
	{
		for (int columns_image = distance; columns_image < original_image->width-distance; columns_image++)
		{
            new_value_R = 0;
            new_value_G = 0;
            new_value_B = 0;
			for (int neighbors_rows = rows_image-distance; neighbors_rows < rows_image+distance+1; neighbors_rows++) //make it always the size of the filter (height)
			{
				for (int neighbors_columns = columns_image-distance; neighbors_columns < columns_image+distance+1; neighbors_columns++) //make it always the size of the filter (width)
				{
                    if (cont < 50)
                    {
                        printf("kk= %d, ", neighbors_rows+distance-rows_image);
                        printf("pp= %d\n", neighbors_columns+distance-columns_image);
                    }
                    cont++;
					new_value_R += original_image->pixels[neighbors_rows][neighbors_columns].data[R] * filter->matrix[neighbors_rows+distance-rows_image][neighbors_columns+distance-columns_image];
                    new_value_G += original_image->pixels[neighbors_rows][neighbors_columns].data[G] * filter->matrix[neighbors_rows+distance-rows_image][neighbors_columns+distance-columns_image];
                    new_value_B += original_image->pixels[neighbors_rows][neighbors_columns].data[B] * filter->matrix[neighbors_rows+distance-rows_image][neighbors_columns+distance-columns_image];
                }
			}
            if (new_value_R > 255){
                new_value_R = 255;
            }
            if (new_value_G > 255){
                new_value_G = 255;
            }
            if (new_value_B > 255){
                new_value_B = 255;
            }
            if (new_value_R < 0){
                new_value_R = 0;
            }
            if (new_value_G < 0){
                new_value_G = 0;
            }
            if (new_value_B < 0){
                new_value_B = 0;
            }
            
            if (cont < 500)
            {
                printf("kk= %f, ", new_value_R);
                
                hola = new_value_R;
                printf("pp = %hhu\n", hola);
            }
            cont++;
            
            filtered_image.pixels[rows_image][columns_image].data[R] = new_value_R;
            filtered_image.pixels[rows_image][columns_image].data[G] = new_value_G;
            filtered_image.pixels[rows_image][columns_image].data[B] = new_value_B;
		}
	}
    new_value_B = -30;
    hola = new_value_B;
    printf("pp = %hhu\n", hola);
     // Free the previous memory data
    freeMemory(original_image);
	*original_image = filtered_image;
    //freeMemory(&filtered_image);
}

//Make the duplicate of the structure to preserve the data
void copyStructureData(ppm_t * original_image, ppm_t * filtered_image)
{
    filtered_image->height = original_image->height;
    filtered_image->width = original_image->width;
    strncpy (filtered_image->magic_number, original_image->magic_number, 3);
    filtered_image->max_value = original_image->max_value;
    allocateMemory(filtered_image);
    
    for (int r=0; r<original_image->height; r++)
    {
        for (int c=0; c<original_image->width; c++)
        {
            filtered_image->pixels[r][c].data[R] = original_image->pixels[r][c].data[R];
            filtered_image->pixels[r][c].data[G] = original_image->pixels[r][c].data[G];
            filtered_image->pixels[r][c].data[B] = original_image->pixels[r][c].data[B];
        }
    }
}

*/



/*

//For each created thread, thread function
void * filterTheImageWithThreads(void * arg)
{
    thread_data_t * thread_data = (thread_data_t *)arg;
    double new_value_R;
    double new_value_G;
    double new_value_B;
    int cont = 0;
    int aux = 0;
    if (thread_data->id != 0)
    {
        aux = thread_data->start_height-thread_data->distance+1;
        printf("ID: %d\n", thread_data->id);
    }
    
//#pragma omp parallel for default(none) shared(distance, original_image->pixels) private(new_value_R, new_value_G, new_value_B, filtered_image)
	for (int rows_image = thread_data->distance+aux; rows_image < thread_data->end_height-thread_data->distance; rows_image++)
	{
		for (int columns_image = thread_data->distance; columns_image < thread_data->original_image->width-thread_data->distance; columns_image++)
		{
            new_value_R = 0;
            new_value_G = 0;
            new_value_B = 0;
			for (int neighbors_rows = rows_image-thread_data->distance; neighbors_rows < rows_image+thread_data->distance+1; neighbors_rows++) //make it always the size of the filter (height)
			{
				for (int neighbors_columns = columns_image-thread_data->distance; neighbors_columns < columns_image+thread_data->distance+1; neighbors_columns++) //make it always the size of the filter (width)
				{
					new_value_R += thread_data->original_image->pixels[neighbors_rows][neighbors_columns].data[R] * thread_data->filter->matrix[neighbors_rows+thread_data->distance-rows_image][neighbors_columns+thread_data->distance-columns_image];
                    new_value_G += thread_data->original_image->pixels[neighbors_rows][neighbors_columns].data[G] * thread_data->filter->matrix[neighbors_rows+thread_data->distance-rows_image][neighbors_columns+thread_data->distance-columns_image];
                    new_value_B += thread_data->original_image->pixels[neighbors_rows][neighbors_columns].data[B] * thread_data->filter->matrix[neighbors_rows+thread_data->distance-rows_image][neighbors_columns+thread_data->distance-columns_image];
                }
			}
            if (new_value_R > 255){
                new_value_R = 255;
            }
            if (new_value_G > 255){
                new_value_G = 255;
            }
            if (new_value_B > 255){
                new_value_B = 255;
            }
            if (new_value_R < 0){
                new_value_R = 0;
            }
            if (new_value_G < 0){
                new_value_G = 0;
            }
            if (new_value_B < 0){
                new_value_B = 0;
            }
            thread_data->filtered_image.pixels[cont][columns_image].data[R] = new_value_R;
            thread_data->filtered_image.pixels[cont][columns_image].data[G] = new_value_G;
            thread_data->filtered_image.pixels[cont][columns_image].data[B] = new_value_B;
		}
        cont++;
	}
    cont = 0;
    if (thread_data->id != NUM_THREADS-1)
    {
        aux-=1;
        printf("ID: %d\n", thread_data->id);
    }
    for (int r = thread_data->start_height; r < thread_data->end_height; r++)
    {
        for (int c = thread_data->distance; c < thread_data->original_image->width-thread_data->distance; c++)
        {
            thread_data->original_image->pixels[r][c].data[R] = thread_data->filtered_image.pixels[cont][c].data[R];
            thread_data->original_image->pixels[r][c].data[G] = thread_data->filtered_image.pixels[cont][c].data[G];
            thread_data->original_image->pixels[r][c].data[B] = thread_data->filtered_image.pixels[cont][c].data[B];
        }
        cont++;
    }
    
    // Free the previous memory data
    //freeMemory(original_image);
	//*original_image = filtered_image;
    //freeMemory(&filtered_image);
    pthread_exit(NULL);
}

*/

/*

#include "ppms.h"

///// FUNCTION DEFINITIONS

// Get the memory necessary to store an image of the size indicated in the structure
void allocateMemory(ppm_t * image)
{
    // Allocate the memory for INDEX array
    image->pixels = malloc (image->height * sizeof(pixel_t *));
    // Allocate the memory for all the DATA array
    image->pixels[0] = calloc (image->height * image->width, sizeof(pixel_t));
    
    // Add the rest of the pointers to the INDEX array
    for (int i=1; i<image->height; i++)
    {
        // Add an offset from the beginning of the DATA array
        image->pixels[i] = image->pixels[0] + image->width * i;
    }
}

// Release the memory for the image
void freeMemory(ppm_t * image)
{
    // Free the DATA array
    free (image->pixels[0]);
    // Free the INDEX array
    free (image->pixels);

    // Set the values for an empty image
    image->width = 0;
    image->height = 0;
    image->pixels = NULL;
}

// Open an image file and copy its contents to the structure
void readImageFile(char * filename, ppm_t * image)
{
    FILE * file_ptr = NULL;
    
    file_ptr = fopen (filename, "r");
    if (!file_ptr)
    {
        printf("Unable to open the file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
    
    // Get the data from the header
    fscanf (file_ptr, "%s", image->magic_number);
    fscanf (file_ptr, "%d", &image->width);
    fscanf (file_ptr, "%d", &image->height);
    fscanf (file_ptr, "%d", &image->max_value);
    // Get rid of the '\n' after max value and before the real data
    fgetc(file_ptr);
    
    // Get the memory for the image data
    allocateMemory(image);
    
    if (!strncmp(image->magic_number, "P3", 3))
    {
        getAsciiPixels(image, file_ptr);
    }
    else if (!strncmp(image->magic_number, "P6", 3))
    {
        getBinaryPixels(image, file_ptr);
    }
    else
    {
        printf("Not a correct PPM format");
    }

    fclose(file_ptr);
}

// Write the data stored in a structure into an image file
void writeImageFile(char * filename, const ppm_t * image)
{
    FILE * file_ptr = NULL;
    
    file_ptr = fopen (filename, "w");
    if (!file_ptr)
    {
        printf("Unable to open the file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
    
    // Insert the header
    fprintf (file_ptr, "%s\n", image->magic_number);
    fprintf (file_ptr, "%d ", image->width);
    fprintf (file_ptr, "%d\n", image->height);
    fprintf (file_ptr, "%d\n", image->max_value);

    // Write the matrix of data for the pixels
    if (!strncmp(image->magic_number, "P3", 3))
    {
        writeAsciiPixels(image, file_ptr);
    }
    else if (!strncmp(image->magic_number, "P6", 3))
    {
        writeBinaryPixels(image, file_ptr);
    }
    else
    {
        printf("Not a correct PPM format");
    }

    fclose(file_ptr);
}

// Read each of the numerical values for the color components
//  and store them in the arrray for the pixels
void getAsciiPixels(ppm_t * image, FILE * file_ptr)
{
    for (int r=0; r<image->height; r++)
    {
        for (int c=0; c<image->width; c++)
        {
            fscanf (file_ptr, "%hhu", &image->pixels[r][c].data[R]);
            fscanf (file_ptr, "%hhu", &image->pixels[r][c].data[G]);
            fscanf (file_ptr, "%hhu", &image->pixels[r][c].data[B]);
            //fscanf (file_ptr, "%hu", &image->pixels[r][c].data[B]);
        }
    }
}

// Copy the bytes from the image into the data array for the pixels
void getBinaryPixels(ppm_t * image, FILE * file_ptr)
{
    fread (image->pixels[0], sizeof(pixel_t), image->width * image->height, file_ptr);    
}

// Write the numerical values for the color components into
//  the output file, giving them a nice format
void writeAsciiPixels(const ppm_t * image, FILE * file_ptr)
{
    for (int r=0; r<image->height; r++)
    {
        for (int c=0; c<image->width; c++)
        {
            fprintf (file_ptr, "%3hhu ", image->pixels[r][c].data[R]);
            fprintf (file_ptr, "%3hhu ", image->pixels[r][c].data[G]);
            fprintf (file_ptr, "%3hhu\t", image->pixels[r][c].data[B]);
        }
        fprintf(file_ptr, "\n");
    }
}

// Copy the pixel data into the file as bytes
void writeBinaryPixels(const ppm_t * image, FILE * file_ptr)
{
    fwrite (image->pixels[0], sizeof(pixel_t), image->width * image->height, file_ptr);    
}

// Compute the negative colors of an image
void getNegativeImage(ppm_t * image)
{
    for (int r=0; r<image->height; r++)
    {
        for (int c=0; c<image->width; c++)
        {
            // Loop for the 3 color components
            for (int color=0; color<3; color++)
            {
                image->pixels[r][c].data[color] = image->max_value - image->pixels[r][c].data[color];
            }
        }
    }
}

// Print the pixels of the image as hexadecimal on the console
void printPixels(ppm_t * image)
{
    for (int r=0; r<image->height; r++)
    {
        for (int c=0; c<image->width; c++)
        {
            // Loop for the 3 color components
            for (int color=0; color<3; color++)
            {
                printf("%02x ", image->pixels[r][c].data[color]);
            }
            printf("\t");
        }
        printf("\n");
    }
}


// Rotate the image a specified number of degrees
// Modify the structure received as a parameter
void rotateImage(ppm_t * source, int angle)
{
    // Local variable for an image structure
    ppm_t destination = {"", 0, 0, 0, NULL};

    // Copy the header data
    strncpy (destination.magic_number, source->magic_number, 3);
    destination.max_value = source->max_value;

    switch (angle)
    {
        case 90:
            // Invert the sizes
            destination.height = source->width;
            destination.width = source->height;
            // Get the memory for the image data
            allocateMemory(&destination);
            // Apply the rotation
            rotateImage90(&destination, source);
            break;
        case 180:
            // Keep the same image ration
            destination.height = source->height;
            destination.width = source->width;
            // Get the memory for the image data
            allocateMemory(&destination);
            // Apply the rotation
            rotateImage180(&destination, source);
            break;
        case 270:
            // Invert the sizes
            destination.height = source->width;
            destination.width = source->height;
            // Get the memory for the image data
            allocateMemory(&destination);
            // Apply the rotation
            rotateImage270(&destination, source);
            break;
        default:
            printf("Can only rotate the image 90, 180 or 270 degrees. Sorry!\n");
            break;
    }

    // Free the previous memory data
    freeMemory(source);
    // Copy the results back to the pointer received
    *source = destination;
}

// Rotate the image and write the result in another structure
void rotateImage90(ppm_t * destination, const ppm_t * source)
{
    // Prepare the limits for the loops
    int max_row = destination->height - 1;

    // Loops to reallocate the pixels
    for (int row_src=0, col_dst=0; row_src<source->height; row_src++, col_dst++)
    {
        for (int col_src=0, row_dst=max_row; col_src<source->width; col_src++, row_dst--)
        {
            // Loop for the 3 color components
            for (int color=0; color<3; color++)
            {
                destination->pixels[row_dst][col_dst].data[color] = source->pixels[row_src][col_src].data[color];
            }
        }
    }
}

// Rotate the image and write the result in another structure
void rotateImage180(ppm_t * destination, const ppm_t * source)
{
    // Prepare the limits for the loops
    int max_row = destination->height - 1;
    int max_col = destination->width - 1;

    // Loops to reallocate the pixels
    for (int row_src=0, row_dst=max_row; row_src<source->height; row_src++, row_dst--)
    {
        for (int col_src=0, col_dst=max_col; col_src<source->width; col_src++, col_dst--)
        {
            // Loop for the 3 color components
            for (int color=0; color<3; color++)
            {
                destination->pixels[row_dst][col_dst].data[color] = source->pixels[row_src][col_src].data[color];
            }
        }
    }
}

// Rotate the image and write the result in another structure
void rotateImage270(ppm_t * destination, const ppm_t * source)
{
    // Prepare the limits for the loops
    int max_col = destination->width - 1;

    // Loops to reallocate the pixels
    for (int row_src=0, col_dst=max_col; row_src<source->height; row_src++, col_dst--)
    {
        for (int col_src=0, row_dst=0; col_src<source->width; col_src++, row_dst++)
        {
            // Loop for the 3 color components
            for (int color=0; color<3; color++)
            {
                destination->pixels[row_dst][col_dst].data[color] = source->pixels[row_src][col_src].data[color];
            }
        }
    }
}

// Scale an image by a given percentage
// Modify the structure received as a parameter
void scaleImage(ppm_t * source, int scale)
{
    // Local variable for an image structure
    ppm_t destination = {"", 0, 0, 0, NULL};

    // Copy the header data
    strncpy (destination.magic_number, source->magic_number, 3);
    destination.max_value = source->max_value;

    // Keep the same image ratio
    destination.height = source->height * (scale / 100.0);
    destination.width = source->width * (scale / 100.0);

    printf("New width: %d, height: %d\n", destination.width, destination.height);
    // Get the memory for the image data
    allocateMemory(&destination);
    // Apply the scale
    resizeImage(&destination, source, scale);
   
    // Free the previous memory data
    freeMemory(source);
    // Copy the results back to the pointer received
    *source = destination;
}

// Apply the scale to the pixels in an image
void resizeImage(ppm_t * destination, const ppm_t * source, int scale)
{
    // Compute the multiplication factors depending on the relative sizes
    double factor_x = source->width / (double) destination->width;
    double factor_y = source->height / (double) destination->height;
    int sum[3];
    // The sum of the pixels will be multiplied by this constant to get the average
    double divisor = 1 / (factor_x * factor_y);

    // Average the colors only when scaling down
    if (divisor > 1)
    {
        divisor = 1;
    }

#ifdef DEBUG
    printf("Factor_X: %lf | Factor_Y: %lf | Multiplied: %lf\n", factor_x, factor_y, factor_x * factor_y);
    printf("1 / Factor_X: %lf | Factor_Y: %lf | Multiplied: %lf\n", 1/factor_x, 1/factor_y, 1/(factor_x * factor_y));
#endif

    // Loops to fill in the pixels in the resized image
    for (int row_dst=0; row_dst<destination->height; row_dst++)
    {
        for (int col_dst=0; col_dst<destination->width; col_dst++)
        {
#ifdef DEBUG
            printf("DEBUG: Generating pixel %d, %d\n", row_dst, col_dst);
#endif
            // Reset the values for the new pixel
            for (int color=0; color<3; color++)
            {
                sum[color] = 0;
            }
#ifdef DEBUG
            printf("DEBUG: \tCollapsing pixel rows %lf, %lf\n", row_dst*factor_y, (row_dst+1)*factor_y);
#endif
            // Loops to average the pixels in the original image
            for (int row_src=row_dst*factor_y; row_src<(row_dst+1)*factor_y; row_src++)
            {
#ifdef DEBUG
            printf("DEBUG: \tCollapsing pixel columns %lf, %lf\n", col_dst*factor_x, (col_dst+1)*factor_x);
#endif
                for (int col_src=col_dst*factor_x; col_src<(col_dst+1)*factor_x; col_src++)
                {
#ifdef DEBUG
            printf("DEBUG: \tCollapsing source pixel %d, %d\n", row_src, col_src);
#endif
                    // Loop for the 3 color components
                    for (int color=0; color<3; color++)
                    {
                        sum[color] += source->pixels[row_src][col_src].data[color];
                    }
                }
            }

#ifdef DEBUG
            printf("DEBUG: \tsum: %d, %d, %d\n", sum[0], sum[1], sum[2]);
#endif
            // Assign the new color to the destination pixel
            for (int color=0; color<3; color++)
            {
                destination->pixels[row_dst][col_dst].data[color] = sum[color] * divisor;
            }

        }
    }
}

//Read the file with the filter and strore it in the structure
void readFilterImage(char * fileName, struct Filter_Image * filter)
{
	FILE * file = NULL;
	file = fopen(fileName, "rb");; 
	
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
	fclose(file);//closing the file
}

//Free memory allocated for the filter file
void freeFilter(filter_image_t * filter)
{
    for (int i = 0; i < filter->height; i++)
	{
		free(filter->matrix[i]);
	}
    free(filter->matrix);   
}

//filtering the given image with the given filter using manually created threads 
void filterTheImage(ppm_t * original_image, filter_image_t * filter)
{
    pthread_t tid[NUM_THREADS];
    int status;
    int height_jump = original_image->height/NUM_THREADS;
    int start_height_jump = 0;
    int end_height_jump = 0;
    thread_data_t dataThread[NUM_THREADS];
    int aux = 0;
    int distance = filter->height / 2;

    for (int i = 0; i < NUM_THREADS; i++)
    {
        start_height_jump = end_height_jump;
        end_height_jump += height_jump;
        ppm_t filtered_image;//creating a copy from the original image
        copyStructureData(original_image, &filtered_image, start_height_jump, end_height_jump);
        dataThread[i].id = i;
        dataThread[i].distance = distance;
        dataThread[i].start_height = start_height_jump;
        dataThread[i].end_height = end_height_jump;
        dataThread[i].original_image = original_image;
        dataThread[i].filtered_image = filtered_image;
        dataThread[i].filter = filter;

        status = pthread_create(&tid[i], NULL, &filterTheImageWithThreads, &dataThread[i]);
        if (status)
        {
            fprintf(stderr, "ERROR: pthread_create %d\n", status);
            exit(EXIT_FAILURE);
        }
        //printf("Started thread: %d\n", i);
    }
    
    // Wait for the threads to finish
    for (int i = 0; i < NUM_THREADS; i++)
    {
        status = pthread_join(tid[i], NULL);
        if (status)
        {
            fprintf(stderr, "ERROR: pthread_join %d\n", status);
            exit(EXIT_FAILURE);
        }
    }
}

//For each created thread, thread function
void * filterTheImageWithThreads(void * arg)
{
    thread_data_t * thread_data = (thread_data_t *)arg;
    double new_value_R;
    double new_value_G;
    double new_value_B;
    int cont = 0;
    int aux = 0;
    if (thread_data->id != 0)
    {
        aux = thread_data->start_height-thread_data->distance+1;
        printf("ID: %d\n", thread_data->id);
    }
    
//#pragma omp parallel for default(none) shared(distance, original_image->pixels) private(new_value_R, new_value_G, new_value_B, filtered_image)
	for (int rows_image = thread_data->distance+aux; rows_image < thread_data->end_height-thread_data->distance; rows_image++)
	{
		for (int columns_image = thread_data->distance; columns_image < thread_data->original_image->width-thread_data->distance; columns_image++)
		{
            new_value_R = 0;
            new_value_G = 0;
            new_value_B = 0;
			for (int neighbors_rows = rows_image-thread_data->distance; neighbors_rows < rows_image+thread_data->distance+1; neighbors_rows++) //make it always the size of the filter (height)
			{
				for (int neighbors_columns = columns_image-thread_data->distance; neighbors_columns < columns_image+thread_data->distance+1; neighbors_columns++) //make it always the size of the filter (width)
				{
					new_value_R += thread_data->original_image->pixels[neighbors_rows][neighbors_columns].data[R] * thread_data->filter->matrix[neighbors_rows+thread_data->distance-rows_image][neighbors_columns+thread_data->distance-columns_image];
                    new_value_G += thread_data->original_image->pixels[neighbors_rows][neighbors_columns].data[G] * thread_data->filter->matrix[neighbors_rows+thread_data->distance-rows_image][neighbors_columns+thread_data->distance-columns_image];
                    new_value_B += thread_data->original_image->pixels[neighbors_rows][neighbors_columns].data[B] * thread_data->filter->matrix[neighbors_rows+thread_data->distance-rows_image][neighbors_columns+thread_data->distance-columns_image];
                }
			}
            if (new_value_R > 255){
                new_value_R = 255;
            }
            if (new_value_G > 255){
                new_value_G = 255;
            }
            if (new_value_B > 255){
                new_value_B = 255;
            }
            if (new_value_R < 0){
                new_value_R = 0;
            }
            if (new_value_G < 0){
                new_value_G = 0;
            }
            if (new_value_B < 0){
                new_value_B = 0;
            }
            thread_data->filtered_image.pixels[cont][columns_image].data[R] = new_value_R;
            thread_data->filtered_image.pixels[cont][columns_image].data[G] = new_value_G;
            thread_data->filtered_image.pixels[cont][columns_image].data[B] = new_value_B;
		}
        cont++;
	}
    cont = 0;
    aux = 0;
    if (thread_data->id != 0)
    {
        aux = thread_data->start_height-thread_data->distance;
    }else
    {
        aux = 0;
    }
    
    for (int r = aux; r < thread_data->end_height; r++)
    {
        for (int c = thread_data->distance; c < thread_data->original_image->width-thread_data->distance-10; c++)
        {
            if (c > 0 && c < thread_data->original_image->width)
            {
                printf("[%d][%d]\n",r,c);
                thread_data->original_image->pixels[r][c].data[R] = thread_data->filtered_image.pixels[cont][c].data[R];
                thread_data->original_image->pixels[r][c].data[G] = thread_data->filtered_image.pixels[cont][c].data[G];
                thread_data->original_image->pixels[r][c].data[B] = thread_data->filtered_image.pixels[cont][c].data[B];
            }
            
            
        }
        cont++;
    }
    
    // Free the previous memory data
    //freeMemory(original_image);
	//*original_image = filtered_image;
    //freeMemory(&filtered_image);
    pthread_exit(NULL);
}

//Make the duplicate of the structure to preserve the data
void copyStructureData(ppm_t * original_image, ppm_t * filtered_image, int start_height, int end_height)
{
    int cont = 0;
    filtered_image->height = end_height-start_height;
    filtered_image->width = original_image->width;
    strncpy (filtered_image->magic_number, original_image->magic_number, 3);
    filtered_image->max_value = original_image->max_value;
    allocateMemory(filtered_image);
    
    for (int r = start_height; r < end_height; r++)
    {
        for (int c = 0; c < original_image->width; c++)
        {
            filtered_image->pixels[cont][c].data[R] = original_image->pixels[r][c].data[R];
            filtered_image->pixels[cont][c].data[G] = original_image->pixels[r][c].data[G];
            filtered_image->pixels[cont][c].data[B] = original_image->pixels[r][c].data[B];
        }
        cont++;
    }
}

*/


