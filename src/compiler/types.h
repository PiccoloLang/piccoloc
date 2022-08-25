
#ifndef TYPES_H
#define TYPES_H

#include <llvm-c/Core.h>

typedef struct {

    LLVMTypeRef voidType;
    LLVMTypeRef boolType;
    LLVMTypeRef intType;
    LLVMTypeRef valType;
    LLVMTypeRef valPtrType;
    LLVMTypeRef strType;
    LLVMTypeRef voidPtrType;
    LLVMTypeRef fnType;
    LLVMTypeRef fnPtrType;

    LLVMTypeRef objType;
    LLVMTypeRef objPtrType;
    #include "types_obj_gen.h"

} types;

void initTypes(types* t);

#endif
