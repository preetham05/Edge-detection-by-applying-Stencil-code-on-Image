/*
 ParallelEdgeDetection.h
 Created by Preetham Kannan on 26/01/18.
 Copyright © 2018 Preetham Kannan. All rights reserved.

 ParallelEdgeDetection.h is to perform convolution on the given matrix.
 */

#ifndef ParallelEdgeDetection_h
#define ParallelEdgeDetection_h

#include<stdio.h>
#include<png.h>
#include<stdlib.h>
#include <omp.h>
#include "EdgeDetectionUtil.h"
#define WIDTH 6000
#define HEIGHT 6000
#define KERNEL_WIDTH 3
#define KERNEL_HEIGHT 3
extern void exitProgram(char text[]);

double parallel_start_time, parallel_end_time;
float parallel_input[HEIGHT][WIDTH], parallel_output[HEIGHT][WIDTH];
int parallel_width, parallel_height;
float parallel_kernel_matrix[KERNEL_HEIGHT][KERNEL_WIDTH] = {-1,-1,-1,-1,8,-1,-1,-1,-1};
png_byte parallel_color_type;
png_byte parallel_bit_depth;
png_bytep *parallel_row_pointers;
png_bytep *parallel_row_pointers1;
png_structp parallel_png_ptr;
png_structp parallel_png_write_ptr;
png_infop parallel_info_ptr;
png_infop parallel_info_write_ptr;
png_byte* parallel_row_ptr;
png_byte* parallel_row_ptr1;
png_byte* parallel_ptr;
png_byte* parallel_ptr1;
 int threads;
char header[8];

/** Method Name : parallel_read_image(inputFileName)
    Method is to read the image from the path and get its basic properties.
    @param *inputFileName: Input filename enterted by the user.
    @return doesnot have return type 
*/
void parallel_read_image(char *inputFileName) {
    int i,j,x,y;
    FILE *fp = fopen(inputFileName, "rb");
    if(fp == NULL){
        exitProgram("File not Found");
    }
    /* Checking the header of PNG file */
    fread(header, 1, 8, fp);
    int ch = !png_sig_cmp(header, 0, 8);
    if (!ch) {
        exitProgram("Not a PNG file");
    }
    /* Reading the image data */
    parallel_png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!parallel_png_ptr) {
        exitProgram("Error occurred during creation of Read Structure");
    }
    parallel_info_ptr = png_create_info_struct(parallel_png_ptr);
    if (!parallel_info_ptr) {
        png_destroy_read_struct(&parallel_png_ptr, NULL, NULL);
        exitProgram("Error occurred during creation of Info Structure");
    }
    png_infop end_info = png_create_info_struct(parallel_png_ptr);
    if (!end_info) {
        png_destroy_read_struct(&parallel_png_ptr, &parallel_info_ptr,NULL);
        exitProgram("Error occurred during creation of Info End Structure");
    }
    if (setjmp(png_jmpbuf(parallel_png_ptr))){
        exitProgram("Error occurred during Initialization \n");
    }
    png_init_io(parallel_png_ptr, fp);
    png_set_sig_bytes(parallel_png_ptr, 8);
    png_read_info(parallel_png_ptr, parallel_info_ptr);
    
    parallel_width = png_get_image_width(parallel_png_ptr, parallel_info_ptr);
    parallel_height = png_get_image_height(parallel_png_ptr, parallel_info_ptr);
    parallel_color_type = png_get_color_type(parallel_png_ptr, parallel_info_ptr);
    parallel_bit_depth = png_get_bit_depth(parallel_png_ptr, parallel_info_ptr);
   
    png_read_update_info(parallel_png_ptr, parallel_info_ptr);
    if (setjmp(png_jmpbuf(parallel_png_ptr))){
        exitProgram("Error occurred while reading image");
    }
    parallel_row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * parallel_height);
    for (j=0; j<parallel_height; j++) {
        parallel_row_pointers[j] = (png_byte*) malloc(png_get_rowbytes(parallel_png_ptr, parallel_info_ptr));
    }
    png_read_image(parallel_png_ptr, parallel_row_pointers);
    for (j=0; j<parallel_height; j++) {
        parallel_row_ptr = parallel_row_pointers[j];
        for ( i=0; i<parallel_width; i++) {
            parallel_ptr = &(parallel_row_ptr[i]);
            parallel_input[i][j]=parallel_ptr[0];
        }
    }
    fclose(fp);
}
/* Convolution Code */
/** Method Name : parallel_image_process()
    Method is to convolute the matrix with the given matrix 3*3 to find out the edge.
    Where  #pragma omp parallel shared(parallel_height,parallel_width ,chunk) shares the value in the whole
    process with different threads and helps in increasing the performance.
    @param no parameters are taken.
    @return doesnot have return type 
*/
void parallel_image_process() {
    
    int i,j,k,y,x;  
   
    parallel_start_time = omp_get_wtime();
    #pragma omp parallel shared(parallel_height,parallel_width ) num_threads(8)
                      {
    #pragma omp for schedule(dynamic)
        for( y = 1; y < parallel_height - 1; y++) {
            for(x = 1; x < parallel_width - 1; x++) {
                parallel_output[y][x] = parallel_kernel_matrix[0][0] * parallel_input[y-1][x-1]
                + parallel_kernel_matrix[0][1] * parallel_input[y][x-1] + parallel_kernel_matrix[0][2] * parallel_input[y+1][x-1]
                + parallel_kernel_matrix[1][0] * parallel_input[y-1][x] + parallel_kernel_matrix[1][1] * parallel_input[y][x]
                + parallel_kernel_matrix[1][2] * parallel_input[y+1][x] + parallel_kernel_matrix[2][0] * parallel_input[y-1][x+1]
                + parallel_kernel_matrix[2][1] * parallel_input[y][x+1] + parallel_kernel_matrix[2][2] *parallel_input[y+1][x+1] ;
                
                parallel_output[y][x] =  parallel_output[y][x] > 255 ? 255 : ( parallel_output[y][x] < 0 ? 0 :  parallel_output[y][x]);
            }
        }
        threads = omp_get_num_threads();
    }
    parallel_end_time = omp_get_wtime();
    printf("Total number of threads: %d \n", threads);
    parallel_row_pointers1 = (png_bytep*) malloc(sizeof(png_bytep) * parallel_height);
    
    for (k = 0; k < parallel_height; k++) {
        parallel_row_pointers1[k] = (png_byte*) malloc(png_get_rowbytes(parallel_png_ptr,parallel_info_ptr));
    }
    for (j = 0; j < parallel_height; j++) {
        parallel_row_ptr1 = parallel_row_pointers1[j];
        for (i = 0; i < parallel_width; i++) {
            parallel_ptr1 = &(parallel_row_ptr1[i]);
            parallel_ptr1[0] = parallel_output[i][j];
        }
    }
}
/** Method Name : parallel_write_image(inputFileName)
    Method is to write into the image file with the name given by user in the console.
    @param *outputFileName: Output filename enterted by the user.
    @return doesnot have return type 
*/
void parallel_write_image(char *outputFileName){
    int j;
    FILE *fp = fopen(outputFileName,"wb");
    if(fp == NULL) {
        exitProgram("Could not create the file");
    }
    parallel_png_write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!parallel_png_write_ptr) {
        exitProgram("Error occurred during write process");
    }
    parallel_info_write_ptr = png_create_info_struct(parallel_png_write_ptr);
    if (!parallel_info_write_ptr) {
        png_destroy_write_struct(&parallel_png_write_ptr, &parallel_info_write_ptr);
        exitProgram("Error occurred during creation of Info Structure");
    }
    png_infop end_write_info = png_create_info_struct(parallel_png_write_ptr);
    if (!end_write_info) {
        png_destroy_write_struct(&parallel_png_write_ptr, &parallel_info_write_ptr);
        exitProgram("Error occurred during creation of Info End Structure");
    }
    png_init_io(parallel_png_write_ptr, fp);
    png_set_IHDR(parallel_png_write_ptr, parallel_info_write_ptr, parallel_width,
                 parallel_height, parallel_bit_depth, parallel_color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    
    png_write_info(parallel_png_write_ptr, parallel_info_write_ptr);
    if (setjmp(png_jmpbuf(parallel_png_write_ptr))) {
        png_destroy_write_struct(&parallel_png_write_ptr, &parallel_info_write_ptr);
        exitProgram("Error occurred during execution");
    }
    png_write_image(parallel_png_write_ptr, parallel_row_pointers1);
    if (setjmp(png_jmpbuf(parallel_png_write_ptr))) {
        exitProgram("Error during writing the image");
    }
    
    png_write_end(parallel_png_write_ptr, parallel_info_write_ptr);
    for(j = 0; j < parallel_height; j++){
        free(parallel_row_pointers[j]);
        free(parallel_row_pointers1[j]);
    }
    free(parallel_row_pointers);
    free(parallel_row_pointers1);
    fclose(fp);
}
/** Method Name : executeParallelEdgeDetection(inputFileName)
    Method is to sync read, imgae process and write.
    @param *inputFileName , *outputFileName: Output filename enterted by the user.
    @return doesnot have return type 
*/
void executeParallelEdgeDetection(char *inputFileName, char *outputFileName) {
    
    parallel_read_image(inputFileName);
    parallel_image_process();
    double parallelComputationTime = (parallel_end_time - parallel_start_time);    
    parallel_write_image(outputFileName);
    printf("The CPU time for Parallel is %f in seconds \n", parallelComputationTime);
    printf("Image created successfully..\n");
}

#endif /* ParallelEdgeDetection_h */
