
#ifndef COMPILER_H
#define COMPILER_H

#include <llvm-c/Core.h>

#include "../package.h"
#include "../ast.h"
#include "rtlib.h"

typedef struct {
    LLVMBuilderRef builder;
    LLVMValueRef func;
    package* pkg;

    LLVMValueRef pkgSrcStr;

    rtlibFuncs* rtlib;
} compiler;

void initCompiler(compiler* comp, LLVMBuilderRef builder, LLVMValueRef func, package* pkg, rtlibFuncs* rtlib);
LLVMValueRef compile(compiler* comp, ast* ast);

#endif
