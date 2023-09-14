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

    //printf("%ld\n", *inputFilesize);
    char* fileContents = read_file(argv[1], inputFilesize);

    //remember that this was to just test if you could print the byte stream and test if it matched what we got from od
    unsigned char* p = (unsigned char*) fileContents;

    // for (int i = 0; i < 16; i++)
    //     printf("%x ", p[i]);
    // printf("\n");

    // for (int j = 16; j < 32; j++) {
    //     printf("%x ", p[j]);
    // }
    // printf("\n");
    //printf("%lld\n", sizeof(fileContents));

    struct WavFile* wavFile = malloc(sizeof(struct WavFile));
    int isValidWavFile = validWavFile(fileContents, *inputFilesize, wavFile);

    if (isValidWavFile != 0) {
        printf("Wav file validation failed. Exiting...\n");
        return isValidWavFile;
    }

    free(inputFilesize);
    printf("Process exited normally\n");
    return 0;
}
