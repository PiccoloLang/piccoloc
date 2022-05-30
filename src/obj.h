
#ifndef OBJ_H
#define OBJ_H

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

#endif
