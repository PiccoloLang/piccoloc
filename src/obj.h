
#ifndef OBJ_H
#define OBJ_H

typedef enum {
    OBJ_STR
} objType;

typedef struct {
    objType type;
} obj;

typedef struct {
    obj obj;
    char* str;
    int len;
} objStr;

#endif
