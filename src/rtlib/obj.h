
#ifndef RTLIB_OBJ_H
#define RTLIB_OBJ_H

#include <stdlib.h>

#include "../obj.h"

obj* allocObj(size_t size, objType type);

#define ALLOC_OBJ(type, typeEnum) (obj ## type*)allocObj(sizeof(obj ## type), OBJ_ ## typeEnum)

#endif
