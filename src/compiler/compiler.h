
#ifndef COMPILER_H
#define COMPILER_H

#include <llvm-c/Core.h>

#include "../package.h"
#include "../ast.h"
#include "rtlib.h"
#include "types.h"
#include "../dynarr.h"

typedef struct {
    token name;
    LLVMValueRef alloc;
    int idx;
    bool initialized; // variables are hoisted - this flag is used to give already initialized/declared variables priority

    bool invalidated; // after an eval operation, all variables in the current scope are no longer guaranteed to be valid,
                      // since they could have been replaced by a dynamic variable.
    
    uint64_t declChrono;
} variable;

typedef struct {
    LLVMModuleRef mod;
    LLVMBuilderRef builder;
    LLVMValueRef func;
    package* pkg;
    LLVMValueRef pkgSrcStr;

    dynarr(variable) vars;

    rtlibFuncs* rtlib;
    types* types;

    bool hadError;

    uint64_t prevEvalChrono;
    int outerScopeVars; // the number of vars in scopes outside the current one
} compiler;

int findVar(compiler* comp, token tkn);

void initCompiler(compiler* comp, LLVMModuleRef mod, LLVMBuilderRef builder, LLVMValueRef func, package* pkg, rtlibFuncs* rtlib, types* types);
void freeCompiler(compiler* comp);
void compileVarDecls(compiler* comp, int firstIdx);
LLVMValueRef compile(compiler* comp, ast* ast);
void compilationError(compiler* comp, token tkn, const char* msg, ...);
void insertDynvarConversion(compiler* comp, ast* expr);

#endif
