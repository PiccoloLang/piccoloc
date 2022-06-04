
#ifndef OBJ_H
#define OBJ_H

#include <stdlib.h>

typedef enum {
    OBJ_STR, OBJ_QUOTE
} objType;

typedef struct {
    objType type;
} obj;

typedef struct {
    obj obj;
    int len;
    char* str;
} objStr;

obj* allocObj(size_t size, objType type);

#define ALLOC_OBJ(type, enum) (obj ## type*)allocObj(sizeof(obj ## type), OBJ_ ## enum)

#endif
