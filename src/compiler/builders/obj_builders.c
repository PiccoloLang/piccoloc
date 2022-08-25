
#include "obj_builders.h"

LLVMValueRef buildObjAlloc(compiler* comp, objType type) {
    int size = 0;
    switch(type) {
        case OBJ_STR: {
            size = sizeof(objStr);
            break;
        }
        case OBJ_FN: {
            size = sizeof(objFn);
            break;
        }
        case OBJ_RTFN: {
            size = sizeof(objRtFn);
            break;
        }
        case OBJ_QUOTE:
            break;
    }    
    LLVMValueRef alloc = buildCall(comp, comp->rtlib->allocateObj.type, comp->rtlib->allocateObj.func, 2, buildInt(comp, type), buildInt(comp, size));

    switch(type) {
        case OBJ_STR:
            return LLVMBuildPointerCast(comp->builder, alloc, comp->types->objStrPtrType, "");
        case OBJ_FN:
            return LLVMBuildPointerCast(comp->builder, alloc, comp->types->objFnPtrType, "");
        case OBJ_RTFN:
            return LLVMBuildPointerCast(comp->builder, alloc, comp->types->objRtFnPtrType, "");
        case OBJ_QUOTE:
            break;
    }
    return alloc; 
}

LLVMValueRef buildGetObjType(compiler* comp, LLVMValueRef objPtr) {
    return buildDeref(comp, LLVMBuildGEP2(comp->builder, comp->types->objType, objPtr, 
                                        (LLVMValueRef[]){LLVMConstInt(LLVMInt32Type(), 0, false), LLVMConstInt(LLVMInt32Type(), 0, false)}, 2, ""));
}

#include "obj_builders_gen.c"
