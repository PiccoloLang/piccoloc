
#include "package.h"

#include <stdio.h>
#include <stdlib.h>

static char* readFile(const char* path) {
    FILE* f = fopen(path, "r");
    if(f == NULL)
        return NULL;
    fseek(f, 0L, SEEK_END);
    size_t fileSize = ftell(f);
    rewind(f);
    char* buffer = (char*)malloc(fileSize + 1);
    if(buffer == NULL)
        return NULL;
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, f);
    buffer[bytesRead] = '\0';
    fclose(f);
    return buffer;
}

bool loadPackage(package* pkg, const char* path) {
    pkg->path = path;
    pkg->src = readFile(path);
    if(pkg->src == NULL)
        return false;
    return true;
}

void freePackage(package* pkg) {
    free(pkg->src);
}
