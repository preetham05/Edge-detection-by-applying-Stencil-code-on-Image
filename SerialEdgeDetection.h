/*
 SerialEdgeDetection.h
 
 Created by Manisha on 25/01/18.
 Copyright © 2018 Manisha. All rights reserved.
 
 SerialEdgeDetection.h is to perform convolution on the given matrix.
 */

#ifndef SerialEdgeDetection_h
#define SerialEdgeDetection_h

#include <stdio.h>
#include <png.h>
#include <stdlib.h>
#include <time.h>
#include "EdgeDetectionUtil.h"
#define WIDTH 6000
#define HEIGHT 6000
#define KERNEL_WIDTH 3
#define KERNEL_HEIGHT 3
extern void exitProgram(char text[]);
extern void serial_read_image(char *inputFileName);

clock_t serial_start_time, serial_end_time;
float serial_input[HEIGHT][WIDTH], serial_output[HEIGHT][WIDTH];
int serial_width, serial_height;
float serial_kernel_matrix[KERNEL_HEIGHT][KERNEL_WIDTH]={-1,-1,-1,-1,8,-1,-1,-1,-1};
png_byte serial_color_type;
png_byte serial_bit_depth;
png_bytep *serial_row_pointers;
png_bytep *serial_row_pointers1;
png_structp serial_png_ptr;
png_structp serial_png_write_ptr;
png_infop serial_info_ptr;
png_infop serial_info_write_ptr;
png_byte* serial_row_ptr;
png_byte* serial_row_ptr1;
png_byte* serial_ptr;
png_byte* serial_ptr1;
char serial_header[8];

/** Method Name : serial_read_image(inputFileName)
    Method is to read the image from the path and get its basic properties.
    @param *inputFileName: Input filename enterted by the user.
    @return doesnot have return type 
*/
void serial_read_image(char *inputFileName) {
    int i,j,k;
    FILE *fp = fopen(inputFileName,"rb");
    if (fp == NULL) {
        exitProgram("File not Found");
    }
    /* Checking the header of PNG file */
    fread(serial_header, 1, 8, fp);
    
    int ch = !png_sig_cmp(serial_header, 0, 8);
    if (!ch) {
        exitProgram("Not a PNG file");
    }
    /* Reading the image data */
    serial_png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!serial_png_ptr){
        exitProgram("Error occurred during creation of Read Structure");
    }
    serial_info_ptr = png_create_info_struct(serial_png_ptr);
    serial_info_ptr = png_create_info_struct(serial_png_ptr);
    if (!serial_info_ptr) {
        png_destroy_read_struct(&serial_png_ptr, NULL, NULL);
        exitProgram("Error occurred during creation of Info Structure");
    }
       
    if (setjmp(png_jmpbuf(serial_png_ptr))){
        exitProgram("Error occurred during Initialization \n");
    }
    png_init_io(serial_png_ptr, fp);
    png_set_sig_bytes(serial_png_ptr, 8);
    png_read_info(serial_png_ptr, serial_info_ptr);
    
    serial_width = png_get_image_width(serial_png_ptr, serial_info_ptr);
    serial_height = png_get_image_height(serial_png_ptr, serial_info_ptr);
    serial_color_type = png_get_color_type(serial_png_ptr, serial_info_ptr);
    serial_bit_depth = png_get_bit_depth(serial_png_ptr, serial_info_ptr);
    
    png_read_update_info(serial_png_ptr, serial_info_ptr);
    if (setjmp(png_jmpbuf(serial_png_ptr))){
        exitProgram("Error occurred while reading image");
    }
    serial_row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * serial_height);
    for (k = 0; k < serial_height; k++) {
        serial_row_pointers[k] = (png_byte*) malloc(png_get_rowbytes(serial_png_ptr, serial_info_ptr));
    }
    png_read_image(serial_png_ptr, serial_row_pointers);
    for ( j = 0; j < serial_height; j++) {
        serial_row_ptr = serial_row_pointers[j];
        for (i = 0; i < serial_width; i++) {
            serial_ptr = &(serial_row_ptr[i]);
            serial_input[i][j] = serial_ptr[0];
        }
    }
    fclose(fp);
}
/** Method Name : serial_image_process()
    Method is to convolute the matrix with the given matrix 3*3 to find out the edge.
    @param no parameters are taken.
    @return doesnot have return type 
*/
void serial_image_process() {
    int i,j,k,x,y;
    serial_start_time = clock();
    /* Convolution Code */
    for( y = 1; y < serial_height; y++) {
        for( x = 1; x < serial_width; x++) {
            serial_output[y][x] = serial_kernel_matrix[0][0] * serial_input[y-1][x-1]
            + serial_kernel_matrix[0][1] * serial_input[y][x-1] + serial_kernel_matrix[0][2] * serial_input[y+1][x-1]
            + serial_kernel_matrix[1][0] * serial_input[y-1][x] + serial_kernel_matrix[1][1] * serial_input[y][x]
            + serial_kernel_matrix[1][2] * serial_input[y+1][x] + serial_kernel_matrix[2][0] * serial_input[y-1][x+1]
            + serial_kernel_matrix[2][1] * serial_input[y][x+1] + serial_kernel_matrix[2][2] *serial_input[y+1][x+1];
            
            serial_output[y][x] =  serial_output[y][x] > 255 ? 255 : (serial_output[y][x] < 0 ? 0 :  serial_output[y][x]);
        }
    }
    serial_end_time = clock();
    double duration = (serial_end_time - serial_start_time) / ((double)CLOCKS_PER_SEC);
    printf("The running time is %lf in seconds \n", duration);
   
    serial_row_pointers1 = (png_bytep*) malloc(sizeof(png_bytep) * serial_height);
    for ( k = 0; k < serial_height; k++) {
        serial_row_pointers1[k] = (png_byte*) malloc(png_get_rowbytes(serial_png_ptr, serial_info_ptr));
    }
    for (j = 0; j < serial_height; j++) {
        serial_row_ptr1 = serial_row_pointers1[j];
        for (i = 0; i < serial_width; i++) {
            serial_ptr1 = &(serial_row_ptr1[i]);
            serial_ptr1[0] = serial_output[i][j];
        }
    }
}

/** Method Name : serial_write_image(inputFileName)
    Method is to write into the image file with the name given by user in the console.
    @param *outputFileName: Output filename enterted by the user.
    @return doesnot have return type 
*/

void serial_write_image(char *outputFileName){
    int j;
    FILE *fp = fopen(outputFileName,"wb");
    if(fp == NULL){
        exitProgram("Could not create the file");
    }
    serial_png_write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!serial_png_write_ptr){
        exitProgram("Error occurred during write process");
    }
    serial_info_write_ptr = png_create_info_struct(serial_png_write_ptr);
    if (!serial_info_write_ptr) {
        png_destroy_write_struct(&serial_png_write_ptr, &serial_info_write_ptr);
        exitProgram("Error occurred during creation of Info Structure");
    }
    png_infop end_write_info = png_create_info_struct(serial_png_write_ptr);
    if (!end_write_info) {
        png_destroy_write_struct(&serial_png_write_ptr, &serial_info_write_ptr);
        exitProgram("Error occurred during creation of Info End Structure");
    }
    png_init_io(serial_png_write_ptr, fp);
    png_set_IHDR(serial_png_write_ptr, serial_info_write_ptr, serial_width,
                 serial_height, serial_bit_depth, serial_color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    
    png_write_info(serial_png_write_ptr, serial_info_write_ptr);
    if (setjmp(png_jmpbuf(serial_png_write_ptr))) {
        png_destroy_write_struct(&serial_png_write_ptr, &serial_info_write_ptr);
        exitProgram("Error occurred during execution");
    }
    png_write_image(serial_png_write_ptr, serial_row_pointers1);
    if (setjmp(png_jmpbuf(serial_png_write_ptr))){
        exitProgram("Error during writing the image");
    }
    png_write_end(serial_png_write_ptr, serial_info_write_ptr);
    for(j = 0; j < serial_height; j++){
        free(serial_row_pointers[j]);
        free(serial_row_pointers1[j]);
    }
    free(serial_row_pointers);
    free(serial_row_pointers1);
    fclose(fp);
}
/** Method Name : executeParallelEdgeDetection(inputFileName)
    Method is to sync read, imgae process and write.
    @param *inputFileName , *outputFileName: Output filename enterted by the user.
    @return doesnot have return type 
*/
void executeSerialEdgeDetection(char *inputFileName, char *outputFileName) {
    serial_read_image(inputFileName);
    serial_image_process();
    serial_write_image(outputFileName);
    printf("Image generated succefully by Serial computation.. \n");
}



#endif /* SerialEdgeDetection_h */
