//Adam Winebarger & Dennis Salo
//CIS 343, Woodring
//Assingment 1: Wav reversser
//This project is designed to reverse wav files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wav.h"
#include "file_lib.h"

void freeWavFile(size_t* inputFileSize, char* fileContents, struct WavFile* waveFile) {
    if (inputFileSize != NULL)
        free(inputFileSize);

    if (fileContents != NULL)
        free(fileContents);

    if (waveFile != NULL) {
        if (waveFile->dataLoc != NULL)
            free(waveFile->dataLoc);
        if (waveFile->header != NULL)
            free(waveFile->header);
        free(waveFile);
    }
}


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
        free(inputFilesize);
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
        freeWavFile(inputFilesize, fileContents, wavFile);
        return isValidWavFile;
    }

    // printf("%d\n", wavFile->headerSize);
    // printf("%lld\n", wavFile->fileSize);
    // printf("%lld\n", wavFile->dataSize);

    //Ok. So now we need to take the data section of our wav struct and reverse it. Seems like the easiest way to do that is going to be to memcopy to a
    //char*, then have an int* interpret the data and cycle through using a 4 loop with the end condition being data size / 4... I think.
    //I'm not quite articulating this as good as I'd like to but I think I've got it down.

    int* dataAsInts = (int*) wavFile->dataLoc;
    long dataSizeAsInts = wavFile->dataSize / 4;
    printf("%d\n", dataSizeAsInts);

    int* reversedDataAsInts = malloc(sizeof(int) * dataSizeAsInts);

    for (int i = 0; i < dataSizeAsInts; i++) {
        reversedDataAsInts[i] = dataAsInts[dataSizeAsInts-i-1];
    }

    char* reversedData = (char*) reversedDataAsInts;

    // for (int i = 0; i < 5; i++) {
    //     for (int j = 0; j < 16; j++) {
    //         printf("%d ", reversedData[16*i + j]);
    //     }
    //     printf("\n");
    // }

    char *reversedDataWithHeader = malloc(sizeof(char) * *inputFilesize);
    memcpy(reversedDataWithHeader, wavFile->header, wavFile->headerSize);
    memcpy(reversedDataWithHeader+wavFile->headerSize, reversedData, wavFile->dataSize);

    write_file(argv[2], reversedDataWithHeader, *inputFilesize);

    //Freeing our malloc'd stuff here
    freeWavFile(inputFilesize, fileContents, wavFile);
    //free(dataAsInts);
    free(reversedDataAsInts);
    free(reversedDataWithHeader);

    printf("Process exited normally\n");
    return 0;
}
