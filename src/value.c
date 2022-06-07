
#include "value.h"
#include "obj.h"

static char* getObjTypename(obj* obj) {
    if(obj->type == OBJ_STR)
        return "string";
    return "";
}

const char* getTypename(val val) {
    if(IS_NUM(val))
        return "number";
    else if(IS_BOOL(val))
        return "bool";
    else if(IS_NIL(val))
        return "nil";
    else if(IS_OBJ(val))
        return getObjTypename(AS_OBJ(val));
    return "";
}
