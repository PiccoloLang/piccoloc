
#ifndef VALUE_H
#define VALUE_H

#include <stdint.h>
#include <stdbool.h>

#define QNAN ((uint64_t)0x7ffc000000000000)

typedef uint64_t val;

static inline val doubleToBits(double a) {
    union {
        double val;
        val bits;
    } data;
    data.val = a;
    return data.bits;
}

static inline double bitsToDouble(val v) {
    union {
        double val;
        val bits;
    } data;
    data.bits = v;
    return data.val;
}

#define NUM_VAL(val) (doubleToBits(val))
#define IS_NUM(val) (((val) & QNAN) != QNAN)
#define AS_NUM(val) (bitsToDouble(val))

#define TAG_NIL 1
#define NIL_VAL() (QNAN | TAG_NIL)
#define IS_NIL(val) ((val) == NIL_VAL())

#define TAG_FALSE 2
#define TAG_TRUE 3
#define BOOL_VAL(val) (QNAN | TAG_FALSE | (val))
#define IS_BOOL(val) ((val | 1) == BOOL_VAL(true))
#define AS_BOOL(val) ((val) == BOOL_VAL(true))

#define TAG_UNINIT 4
#define UNINIT_VAL() (QNAN | TAG_UNINIT)
#define IS_UNINIT(val) ((val) == UNINIT_VAL()) 

#define TAG_OBJ (QNAN | ((uint64_t)1 << 63))
#define OBJ_VAL(val) (TAG_OBJ | (uint64_t)(intptr_t)(val))
#define IS_OBJ(val) (((val) & TAG_OBJ) == TAG_OBJ)
#define AS_OBJ(val) (void*)(intptr_t)(val & ~TAG_OBJ)

const char* getTypename(val val);

#endif
