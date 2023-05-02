 /*
 MPI_Edge_Detection.c
 Created by Preetham Kannan on 26/01/18.
 Copyright � 2018 Preetham Kannan. All rights reserved.

 MPI_Edge_Detection.c is to perform convolution on the given matrix based on the Message Passing Interface.
 */
 
#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include<png.h>
#define MASTER 0
#include "EdgeDetectionUtil.h"
#define mpi_width 6000
#define mpi_height 6000
#define KERNEL_MPI_WIDTH 3
#define KERNEL_MPI_HEIGHT 3

int mpitasks,nthreads,taskid,numworkers,source,dest,mtype,rows,averow,extra,offset,len;	//declaring variables for MPI
float Img[mpi_height][mpi_width],Conv[mpi_height][mpi_width];						//declaring input and output arrays
int width, height;					//declaring the variables for manupulating the image
float sum[mpi_height][mpi_width];	
png_byte color_type;										//declaring png variables
png_byte bit_depth;
png_structp png_ptr;
png_infop info_ptr;
png_bytep * row_pointers;
png_byte* row;
png_byte* row1;
png_byte* ptr;
int nthreads,tid,chunk=100;
png_byte* ptr1;
int kernelmatrix[KERNEL_MPI_HEIGHT][KERNEL_MPI_WIDTH]={-1,-1,-1,				//stencil kernel
		-1, 8,-1,
		-1,-1,-1};
double mpi_start, mpi_end,mpi_duration;			//declaring variables for calculating runtime
char header[8];					//declaring variable to check the signature of png image
FILE *fp;					//file pointer 
MPI_Status status;
char hostname[MPI_MAX_PROCESSOR_NAME];
int main (int argc, char *argv[])
{
    int x,y;
	printf("MPI execution starting .. \n");
	fp = fopen("test-image.png", "rb" "wb");
	if (!fp)
	{
		exitProgram("File not found.\n");
		
	}
	
	/*first 1 to 8 bytes of the image file is passed to function png_sig_cmp() to check if it matches to the signature of png image  */
	
	fread(header, 1, 8, fp);
	if (png_sig_cmp(header, 0, 8))
	{
		exitProgram("Not a PNG file\n");
	}
		/* initializing png variables for reading the image */
		
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);		//initializing png_struct for reading the image

	if (!png_ptr)
	{
		exitProgram("Error occurred during creation of Read Structure");
		
	}
	info_ptr = png_create_info_struct(png_ptr);						//initializing png_info for reading the image
	if (!info_ptr)
	{
		exitProgram("Error occurred during creation of Info Structure");
		
	}
	if (setjmp(png_jmpbuf(png_ptr)))							//for error handling
	{
		exitProgram("Error occurred during Initialization \n");
		
	}
	png_init_io(png_ptr, fp);	//Initializing the default input/output functions for the PNG file, file pointer is passed to png_ptr
	png_set_sig_bytes(png_ptr, 8);  // stores the number of bytes of the PNG file signature that have been read from the PNG stream
	png_read_info(png_ptr, info_ptr);

	//Image data from PNG image is read to get image characteristics
	width = png_get_image_width(png_ptr, info_ptr);		//mpi_width of png image is stored
	height = png_get_image_height(png_ptr, info_ptr);	//mpi_height of png image is stored
	color_type = png_get_color_type(png_ptr, info_ptr); 	//color type of png image is stored
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);	//no of bits used in png image is stored
	
	/* Read File */
	row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);		//memory allocation of image
	for (y=0; y<height; y++)
		row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));
		
	png_read_image(png_ptr, row_pointers);							//Entire image pixel values stored into memory
	if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_GRAY)
	{
		exitProgram("Image is not a gray scale image\n");;
	
	}
	for (y=0; y<height; y++)
	{
		row = row_pointers[y];
		for (x=0; x<width; x++)
		{
			ptr = &(row[x]);
			Img[x][y]=ptr[0];						//Pixel values of image from memory written into array Img[][]
		}
	}
	/* Initializing MPI	*/
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
	MPI_Comm_size(MPI_COMM_WORLD,&mpitasks);
	numworkers = mpitasks-1;			//number of process
	printf("MPI_Image Convolution has started with %d tasks.\n",mpitasks);
	/* Master Task */
	
	if (taskid == 0)
	{
		mpi_start= MPI_Wtime();
		
		averow = height/numworkers;
		extra = height%numworkers;
		offset = 0;
		mtype = 1;
		for (dest=1; dest<=numworkers; dest++)
		{
			offset=(dest-1)*averow;
			rows = (dest <= extra) ? averow+1 : averow;
			//printf("Sending %d rows to task %d offset=%d\n",rows,dest,offset);
			MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
			MPI_Send(&rows, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);   // The master task distributes the rows to worker tasks
			MPI_Send(&Img[offset][0], height*rows, MPI_INT, dest, mtype,
					MPI_COMM_WORLD);
		}
		mtype = 2;
		for (x=1; x<=numworkers; x++)
		{
			source = x;
			MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);// The master tasks receives the output matrix from worker tasks
			MPI_Recv(&rows, 1, MPI_INT, source, mtype, MPI_COMM_WORLD,&status);
			MPI_Recv(&Conv[offset][0], width*height, MPI_INT, source, mtype,MPI_COMM_WORLD, &status);
		}
		mpi_end=MPI_Wtime();
		for (y=0; y<height; y++)
		{
			row1 = row_pointers[y];
			for (x=0; x<width; x++)
			{
				ptr1 = &(row1[x]);
				ptr1[0]=Conv[x][y];			//Writing the Conv[][] array into memory for displaying the image
			}
		}
		/* create file */
		fp = fopen("test_output.png", "wb");
		if (!fp)
		{
			exitProgram("Could not create the file");
		}
		
		/* initializing png variables for writing the image */
		
		png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);	//initializing png_struct for writing the image
		if (!png_ptr)
		{
			exitProgram("Error occurred during write process");
		}
		info_ptr = png_create_info_struct(png_ptr);				//initializing png_info for writing the image
		if (!info_ptr)
		{
			exitProgram("Error occurred during creation of Info Structure");
		}
		if (setjmp(png_jmpbuf(png_ptr)))						//error handling
		{		
			exitProgram("Error occurred during execution");
		}
		png_init_io(png_ptr, fp);	//Initializing the default input/output functions for the PNG file, file pointer is passed to png_ptr
		
		//setting the information of the output image
		
		png_set_IHDR(png_ptr, info_ptr, width, height,bit_depth, color_type, PNG_INTERLACE_NONE,PNG_COMPRESSION_TYPE_BASE,PNG_FILTER_TYPE_BASE);
		
		png_write_info(png_ptr, info_ptr);			//Image data from PNG image is written
		png_write_image(png_ptr, row_pointers);		//image is written
		png_write_end(png_ptr, info_ptr);
		for(y=0;y<height;y++)						//freeing the memory allocations
			free(row_pointers[y]);
		free(row_pointers);
		fclose(fp);
		printf("The run time of MPI and openMP is %f sec\n",mpi_end-mpi_start);
	}
	/* Slave Task */
	if (taskid>0)
	{
		mtype = 1;
		MPI_Get_processor_name(hostname, &len);
		printf ("Task %d with %d rows is running on %s\n",taskid,rows,hostname);
		MPI_Recv(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
		MPI_Recv(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
		MPI_Recv(&Img[offset][0], width*height, MPI_INT, MASTER, mtype, MPI_COMM_WORLD,&status);
		/*	Convolution	*/	
#pragma omp parallel shared(height,width ,chunk) num_threads(16)
      {
	    #pragma omp for schedule(dynamic,chunk) nowait
	for(y=0;y<height;y++)
		{
			for(x=offset+1;x<(rows*taskid);x++)
			{
		Conv[x][y] = kernelmatrix[0][0] * Img[x-1][y-1]
                + kernelmatrix[0][1] * Img[x][y-1] + kernelmatrix[0][2] * Img[x+1][y-1]
                + kernelmatrix[1][0] * Img[x-1][y] + kernelmatrix[1][1] * Img[x][y]
                + kernelmatrix[1][2] * Img[x+1][y] + kernelmatrix[2][0] * Img[x-1][y+1]
                + kernelmatrix[2][1] * Img[x][y+1] + kernelmatrix[2][2] * Img[x+1][y+1] ;
                
                Conv[x][y] =  Conv[x][y] > 255 ? 255 : ( Conv[x][y] < 0 ? 0 :  Conv[x][y]);			
			}
		 }
	}
			mtype = 2;
		MPI_Send(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
		MPI_Send(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
		MPI_Send(&Conv[offset][0], height*rows, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
	}
	MPI_Finalize();
}

