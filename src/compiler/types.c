
#include "types.h"
#include <stdbool.h>
#include <stdlib.h>

void initTypes(types* t) {

    t->voidType = LLVMVoidType();
    t->boolType = LLVMInt1Type();
    t->intType = LLVMInt32Type();
    t->valType = LLVMInt64Type();
    t->valPtrType = LLVMPointerType(t->valType, 0);
    t->strType = LLVMPointerType(LLVMInt8Type(), 0);
    t->voidPtrType = LLVMPointerType(t->voidType, 0);
    t->fnType = LLVMFunctionType(t->valType, (LLVMTypeRef[]){t->valPtrType}, 1, false);
    t->fnPtrType = LLVMPointerType(t->fnType, 0);

    t->objType = LLVMStructType((LLVMTypeRef[]){LLVMInt8Type()}, 1, false);
    t->objPtrType = LLVMPointerType(t->objType, 0);
    #include "types_obj_gen.c"

}
