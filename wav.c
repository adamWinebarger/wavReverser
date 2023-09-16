#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef union { //This actually might not end up being needed. But it was *a* way of converting 4 chars to an int
    char c[4];
    int i;
} raw_int;

//It seems like there's a good bit of information we need to store pertaining to the wavFiles so I built this struct
struct WavFile {
    long long fileSize, dataSize; //in bytes
    char* dataLoc;
    char* header;
    int headerSize; //we'll throw padding in here as well.
};

int findDataInMetadataSection(const char* fileInput, int fileSize, int* metadataSize) {
    int j;

    //Since we've already gone in 22 deep there's no reason to go much further than that.
    for (int i = 22; i < fileSize-22; i++) {
        if (fileInput[i] == 'd') {    //So this should start cycling any time it finds a d, then check if the string is equal to data, then save the next 4 bytes after data into an array if it finds it.
            //printf("%s\n", fileInput+i);
            //This is extremely crude but strstr was not wanting to cooperate here
            if (fileInput[i+1] == 'a' && fileInput[i+2] == 't' && fileInput[i+3] == 'a'
                && fileInput[i+7] == fileInput[7] && fileInput[i+6] == fileInput[6]
                //This checks the 16 most significant bits against the ones in the file size section for a match
                //(that way if the word data occurs earlier on then it... at least is less likely to lead to undefined behavior)
                //we could check the 24 most significant bits but then it will fail if the header is bigger than 255 Bytes and one of our test files has a
                //242B header so I feel like it could be possible.
            ) {
                //printf("In strstr");
                char dataChars[4] = {fileInput[i+4], fileInput[i+5], fileInput[i+6], fileInput[i+7]}; //since the first is more than likely 00, we can't use strstr
                int* p = (int*) dataChars;
                //printf("datachars: %d\n", *p);
                *metadataSize = *p;
                //printf("%d\n", *metadataSize);
                return 1; //Indicates success
            }
        }

        if (fileInput[i] == '\0') {
            int j;
            for (j = 0; j < 16; j++) {
                //printf("%d\n", j);
                if (fileInput[i+j] != '\0')
                    break;
            //Since the header section adds an entire row of all 00's after the metadata concludes, this should exit us when we reach the conclusion of the header file.
            }
            if (j == 16) {
                printf("Reached padding section of the header. Exiting\n");
                return 0; //indicates failure
            }
        }
    }
    //Theoretically unreachable code.
    printf("This shouldn't be reachable. If you're seeing this, it means something went wrong.\n");
    return 0;
}

int validWavFile(const char* fileInput, int fileSize, struct WavFile* wavFilePTR) {

    char *checkStrings[3];
    checkStrings[0] = "RIFF";
    checkStrings[1] = "WAVEfmt";
    checkStrings[2] = "data"; //this ended up not being needed. Scratch that. It will still be useful. Scratch that again. We aren't using if


    //basic check. Can't be a wav file if the metadata is too small... I think
    if (fileSize <= 45) {
        printf("File/header too small to be a .wav file. Exiting...\n");
        return 9;
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

    //So now we need to find "data"
    int *dataSize = malloc(sizeof(int));

    if (!findDataInMetadataSection(fileInput, fileSize, dataSize)) {
        printf("Unable to find data. Exiting\n");
        free(dataSize);
        return 11;
    }

    //printf("%d\n", fileSize - *dataSize);
    //printf("Datasize mod 4: %d\n", *dataSize % 4);

    int headerSize = fileSize - *dataSize; //this should tell us to interpret them as an integer
   // printf("Header size mod 4: %d\n", headerSize % 4);

    //Putting all of the relevant data into our struct here so that we're doing it all in one place.
    wavFilePTR->headerSize = headerSize;
    wavFilePTR->dataSize = *dataSize;
    wavFilePTR->fileSize = fileSize;
    //printf("Made it to here\n");

    char* data = malloc(sizeof(char) * *dataSize);
    memcpy(data, fileInput+headerSize, sizeof(char) * *dataSize);
    char* header = malloc(sizeof(char) * headerSize);
    memcpy(header, fileInput, sizeof(char) * headerSize);

    wavFilePTR->dataLoc = data;
    wavFilePTR->header = header;

    free(dataSize);
    printf("Wav Validator succeeded\n");
    return 0;
}
