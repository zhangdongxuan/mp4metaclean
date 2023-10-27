//
//  main.cpp
//  metacleaner
//
//  Created by disenzhang on 2023/9/22.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "MP4ParseContext.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    MP4ParseContext *context = new MP4ParseContext("/Users/disenzhang/Downloads/IMG_3767_副本.MOV");
    context->clearMetaValues();
    
    
    FILE *fileHandle = fopen("/Users/disenzhang/Downloads/IMG_33.dat", "rb+");
    if (fileHandle == NULL) {
        fileHandle = fopen("/Users/disenzhang/Downloads/IMG_33.dat", "w+");
        void *buff = malloc(1024);
        memset(buff, 17, 1024);
        fwrite(buff, 1024, 1, fileHandle);
        fclose(fileHandle);
        return 0;
    }

    int size = 32;
    void *buff = malloc(size);
    fseek(fileHandle, 16, SEEK_SET);
    printf("current offset:%ld \n", ftell(fileHandle));
    fread(fileHandle, size, 1, fileHandle);
    printf("current offset:%ld \n", ftell(fileHandle));
    
    printf("current offset:%ld \n", ftell(fileHandle));
    fseek(fileHandle, -5, SEEK_CUR);
    printf("current offset:%ld \n", ftell(fileHandle));
    
    memset(buff, 0, size);
    fwrite(buff, size, 1, fileHandle);
    fclose(fileHandle);
    
    
    return 0;
}
