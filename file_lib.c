#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

//This function will grab us the size of the file in Bytes, which is something that we'll need later
size_t* getFileSize(const char* filePath) {

    size_t size;
    FILE* file = fopen(filePath, "r");
    if (file == NULL)
        return NULL;

    fseek(file, 0L, SEEK_END); //Basically just counts from 0 to end... I wonder if there's a better way to do this

    size = ftell(file); //gets us the number of Bytes
    // printf("%ld\n", size);
    fclose(file);

    size_t* sizeptr = malloc(size); //assigning the value of size to a size_t*
    *sizeptr = size;    //Kind of woks out since read_file needs a size_t* anyways.

    //free(file);
    return sizeptr;
}

char* read_file(const char* path, size_t* fileSize) {
    FILE* file = fopen(path, "rb"); // Open the file in binary mode
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    *fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for the array of bytes
    char* byteArray = malloc(*fileSize);
    if (byteArray == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }

    // Read the file content into the array
    size_t bytesRead = fread(byteArray, 1, *fileSize, file);
    if (bytesRead != *fileSize) {
        perror("Error reading file");
        fclose(file);
        free(byteArray);
        return NULL;
    }

    fclose(file);
    return byteArray;
}

// Function to write an array of bytes to a file
int write_file(const char* path, const char* byteArray, size_t byteArraySize) {
    FILE* file = fopen(path, "wb"); // Open the file in binary write mode
    if (file == NULL) {
        perror("Error opening file");
	return errno;
    }

    // Write the byteArray to the file
    size_t bytesWritten = fwrite(byteArray, 1, byteArraySize, file);
    if (bytesWritten != byteArraySize) {
        perror("Error writing to file");
        fclose(file);
	return errno;
    }

    fclose(file);
    return 0;
}

