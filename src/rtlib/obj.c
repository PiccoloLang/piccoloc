
#include "obj.h"

obj* allocObj(size_t size, objType type) {
    obj* res = malloc(sizeof(obj));
    res->type = type;
    return res;
}