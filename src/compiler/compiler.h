
#ifndef COMPILER_H
#define COMPILER_H

#include <llvm-c/Core.h>

#include "../package.h"
#include "../ast.h"
#include "rtlib.h"
#include "../dynarr.h"

typedef struct {
    token name;
    LLVMValueRef alloc;
    int idx;
} variable;

typedef struct {
    LLVMBuilderRef builder;
    LLVMValueRef func;
    package* pkg;
    LLVMValueRef pkgSrcStr;

    dynarr(variable) vars;

    rtlibFuncs* rtlib;
} compiler;

int findVar(compiler* comp, token tkn);

void initCompiler(compiler* comp, LLVMBuilderRef builder, LLVMValueRef func, package* pkg, rtlibFuncs* rtlib);
void freeCompiler(compiler* comp);
void compileVarDecls(compiler* comp, int firstIdx);
LLVMValueRef compile(compiler* comp, ast* ast);

#endif
