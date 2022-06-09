
#include "builders.h"
#include "../compiler.h"

LLVMValueRef buildVarDecl(compiler* comp, astVarDecl* varDecl) {
    int idx = findVar(comp, varDecl->name);
    variable* var = &comp->vars.vals[idx];
    LLVMValueRef expr = compile(comp, varDecl->expr);
    buildSetPtr(comp, var->alloc, compile(comp, varDecl->expr));
    return expr;
}
