
#include "builders.h"
#include "../compiler.h"

#include <stdio.h>

LLVMValueRef buildVarDecl(compiler* comp, astVarDecl* varDecl) {
    int idx = varDecl->idx;
    variable* var = &comp->vars.vals[idx];
    var->initialized = true;
    var->invalidated = false;
    LLVMValueRef expr = compile(comp, varDecl->expr);
    buildSetPtr(comp, var->alloc, expr);
    return expr;
}
