#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef union {
    char c[4];
    int i;
} raw_int;

struct WavFile {
    char header[45];
    long long fileSize; //in bytes
    char *data;
};

int validWavFile(const char* fileInput, int fileSize, struct WavFile *wavFile) {

    char *checkStrings[3];
    checkStrings[0] = "RIFF";
    checkStrings[1] = "WAVEfmt";
    checkStrings[2] = "data";


    if (fileSize <= 45) {
        printf("File too small to be a .wav file. Exiting...\n");
        return 9;
    }

    char header[46]; //We'll add a \0 to the end of this just to be safe
    for (int i = 0; i < 45; i++) {
        header[i] = fileInput[i];
    }

    //RIFF check
    if (strncmp(fileInput, checkStrings[0], 4)) {
        printf("RIFF not found. Assuming not a wav File\n");
        return 11;
    }

    //let's do byte size here I guess
    //So the byte sequence that gives us the integer in bytes needs to be read from right to left in order to give the correct sum
    //... so we're going to come bac to that
    char byteSizeBytes[4];
    strncpy(byteSizeBytes, fileInput+4, 4); //So this let's us interpret bytes 4 thru 7 as an integer
    int* byteSize = (int*) byteSizeBytes;
    //printf("%d\n", *byteSize);

    //Size match check check
    if (*byteSize != fileSize - 8) {
        printf("Size does not match the metadata\n");
        return 12;
    }

    //WAVE check and fmt check
    if (strncmp(fileInput+8, checkStrings[1], 7)) {
        printf("WAVEfmt not found\n");
        return 11;
    }

    char channelChars[8];
    strncpy(channelChars, fileInput+22, 8);
    long* channelNumbers = (long*) channelChars;

    //Channel check
    if (*channelNumbers != 2) {
        printf("Channel mismatch detected. Exiting...\n");
        return 12;
    }

    char formatChars[2];
    strncpy(formatChars, fileInput+20, 2);
    short* formatType = (short*) formatChars;

    //Format type check
    if (*formatType != 1) {
        printf("Format mismatch detected. Exiting...\n");
        return 12;
    }

    printf("Wav Validator succeeded\n");
    return 0;
}
