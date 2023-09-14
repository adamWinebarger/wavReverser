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
    checkStrings[2] = "data"; //this ended up not being needed... oh well


    //basic check. Can't be a wav file if the metadata is too small... I think
    if (fileSize <= 45) {
        printf("File too small to be a .wav file. Exiting...\n");
        return 9;
    }

    //char header[45]; //We'll add a \0 to the end of this just to be safe
    for (int i = 0; i < 44; i++) {
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
    int* byteSize = (int*) byteSizeBytes; //basically interpreting 4 chars as an int to make sure the filesize is correct
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

    //So now that we've passed all of our checks, I think now would be a good time to throw relevant data into a handy little struct that we can pass back via pointer

    //Ok. So first we need to determine the size of the header, which is just a matter off assigning a pointer to the first occurence of "data" in our input
    // file, and then looking at the next 4 bytes after that. That will tell us how big the metadata is and then will give us a nice point to cut in order
    // to separate the metadata from the data.

    //Also, looking at the metadata from the different wav files I made, it looks like regardless of the size used for the actual metadata, it adds padding
    // in the form of 00 bytes up to the next multiple of 16, and then another 16 Bytes on top of that (so our wav of "Jerk It Out" used 242 bytes for metadata),
    // but then added padding... It acctually looks like it's adding considerably more padding based on the mem addresses. So I think that we should just round
    //up to the next multiple of 16 and then start working back from there. That way we'll have a litte bit of room for overruns

    printf("Wav Validator succeeded\n");
    return 0;
}
