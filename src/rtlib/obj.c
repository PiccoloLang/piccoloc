
#include "obj.h"
#include <string.h>

obj* allocObj(size_t size, objType type) {
    obj* res = malloc(size);
    memset(res, 0, size);
    res->type = type;
    return res;
}