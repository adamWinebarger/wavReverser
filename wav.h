#ifndef wav_H
#define wav_H

//this might be easier for passing relevant data
struct WavFile {
    char header[45];
    long long fileSize; //in bytes
    char *data;
};

//This will be our checker to make sure that our wav... is valid
int validWavFile(const char* fileInput, int fileSize, struct WavFile *wavFile);

#endif
