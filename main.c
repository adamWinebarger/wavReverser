//Adam Winebarger & Dennis Salo
//CIS 343, Woodring
//Assingment 1: Wav reversser
//This project is designed to reverse wav files
#include <stdio.h>
#include <stdlib.h>

#include "wav.h"
#include "file_lib.h"



int main(int argc, char *argv[])
{
    //So I'm thinking that the way to go here is to pass the input and output files in the args to avoid having
    //to do too much extra work on the C side in order to get these filepaths

    //At a minimum, our program shouold have 3 arguments: Calling the executable, the input file, and the output file (in that order)
    if (argc < 3) {
        printf("Error. Too few arguments\n");
        return 3; //I guess we'll make this our exit code for not enough args.
    }

    //printf("%s\n", argv[1]);
    size_t* inputFilesize = getFileSize(argv[1]);

    if (inputFilesize == NULL) {
        printf("Problem with getting file size. Please check that your input file exists.\n");
        return 4;
    }

    printf("%ld\n", *inputFilesize);

    printf("Process exited normally\n");
    return 0;
}
