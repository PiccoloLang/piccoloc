
#ifndef PACKAGE_H
#define PACKAGE_H

#include <stdbool.h>

typedef struct {
    const char* path;
    char* src;
} package;

bool loadPackage(package* pkg, const char* path);
void freePackage(package* pkg);

#endif
