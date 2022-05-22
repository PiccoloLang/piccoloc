
#ifndef DYNARR_H
#define DYNARR_H

#include <stdlib.h>

#define dynarr(type) struct {type* vals; int cnt; int cap;}

#define initDynarr(arr) \
    do { \
        dynarr(void)* a = arr; \
        a->vals = NULL; \
        a->cnt = 0; \
        a->cap = 0; \
    } while(0);

#define freeDynarr(arr) \
    do { \
        dynarr(void)* a = arr; \
        free(a->vals); \
        a->cnt = 0; \
        a->cap = 0; \
    } while(0);

#define pushDynarr(arr, val) \
    do { \
        (arr)->cnt++; \
        if((arr)->cnt > (arr)->cap) { \
            if((arr)->cap == 0) { \
                (arr)->vals = malloc(8 * sizeof(*(arr)->vals)); \
                (arr)->cap = 8; \
            } else { \
                (arr)->cap *= 2; \
                (arr)->vals = realloc((arr)->vals, sizeof(*(arr)->vals) * (arr)->cap); \
            } \
        } \
        (arr)->vals[(arr)->cnt - 1] = val; \
    } while(0);

#endif
