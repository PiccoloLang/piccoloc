
#ifndef OBJ_H
#define OBJ_H

#include <stdlib.h>
#include "value.h"

#include "obj_enum_gen.h"

typedef struct {
    objType type;
} obj;

typedef val (*fnType)(val* args);

#include "obj_gen.h"

obj* allocObj(size_t size, objType type);

#define ALLOC_OBJ(type, enum) (obj ## type*)allocObj(sizeof(obj ## type), OBJ_ ## enum)

#endif
