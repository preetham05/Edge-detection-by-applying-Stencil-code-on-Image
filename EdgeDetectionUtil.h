/*
 EdgeDetectionUtil.h
 
 Created by Preetham Kannan on 25/01/18.
 Copyright © 2018 Preetham Kannan. All rights reserved.
 
 //This class is to exit the program when error occurs.
 */

#ifndef EdgeDetectionUtil_h
#define EdgeDetectionUtil_h

#include <stdio.h>

void exitProgram(char text[]) {
    
    printf("\nError: %s\n", text);
    exit(0);
}

#endif /* EdgeDetectionUtil_h */
