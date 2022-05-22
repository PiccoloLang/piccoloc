
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "../value.h"
#include "../obj.h"

static void printObj(obj* obj) {
    if(obj->type == OBJ_STR) {
        objStr* str = (objStr*)obj;
        printf("%s\n", str->str);
    }
}

void print(val value) {
    if(IS_NUM(value))
        printf("%g\n", AS_NUM(value));
    else if(IS_BOOL(value))
        printf("%s\n", AS_BOOL(value) ? "true" : "false");
    else if(IS_NIL(value))
        printf("nil\n");
    else if(IS_OBJ(value))
        printObj(AS_OBJ(value));
}

void runtimeError(int lineNum, int offset, const char* line, const char* msg, int argc, ...) {
    printf("Runtime Error: ");
    va_list args;
    va_start(args, argc);
    for(const char* c = msg; *c != '\0'; c++) {
        if(*c == '@') {
            val v = va_arg(args, val);
            printf("%s", getTypename(v));
        } else {
            printf("%c", *c);
        }
    }
    va_end(args);
    printf("\n");
    printf("Line %d| %s\n", lineNum, line);

    int chars = 7;
    while(lineNum > 0) {
        lineNum /= 10;
        chars++;
    }
    for(int i = 0; i < chars + offset; i++)
        printf(" ");
    printf("^\n");
    
    exit(-1);
}

static obj* allocObj(size_t size, objType type) {
    obj* res = malloc(sizeof(obj));
    res->type = type;
    return res;
}

#define ALLOC_OBJ(type, typeEnum) (obj ## type*)allocObj(sizeof(obj ## type), OBJ_ ## typeEnum)

val makeString(const char* str) {
    objStr* strObj = ALLOC_OBJ(Str, STR);
    strObj->len = strlen(str);
    strObj->str = malloc(strObj->len + 1);
    strcpy(strObj->str, str);
    return OBJ_VAL(strObj);
}