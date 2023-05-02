/*
 EdgeDetectionMain.c
 Created by Manisha on 25/01/18.
 Copyright © 2018 Manisha. All rights reserved.

 This is to trigger the serial and parallel computing and generation of the edge detected image.
*/
#include <stdio.h>
#include <string.h>

#include "SerialEdgeDetection.h"
#include "ParallelEdgeDetection.h"

extern void executeSerialEdgeDetection(char *inputFileName, char *outputFileName);
extern void executeParallelEdgeDetection(char *inputFileName, char *outputFileName);

int main(int argc, char *argv[]) {
    
    printf("%s execution starting .. \n", argv[1]);

    if(strcmp(argv[1], "serial") == 0) {
     executeSerialEdgeDetection(argv[2], argv[3]);
    } 
    else {
       
      executeParallelEdgeDetection(argv[2], argv[3]);
    }
    return 0;
}

