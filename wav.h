#ifndef wav_H
#define wav_H

//this might be easier for passing relevant data
struct WavFile {
    long long fileSize, dataSize; //in bytes
    char* dataLoc;
    char* header;
    int headerSize; //we'll throw padding in here as well.
};

//This will be our checker to make sure that our wav... is valid
int validWavFile(const char* fileInput, int fileSize, struct WavFile *wavFile);

#endif
