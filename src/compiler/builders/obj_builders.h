
#ifndef BUILDERS_OBJ_H
#define BUILDERS_OBJ_H

#include "builders.h"
#include "../../obj.h"

LLVMValueRef buildObjAlloc(compiler* comp, objType type);

LLVMValueRef buildGetObjType(compiler* comp, LLVMValueRef objPtr);

#include "obj_builders_gen.h"

#endif
