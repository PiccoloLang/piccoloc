
#include "compiler.h"

#include <stdlib.h>

#include "builders/builders.h"

void initCompiler(compiler* comp, LLVMBuilderRef builder, LLVMValueRef func, package* pkg, rtlibFuncs* rtlib) {
    comp->builder = builder;
    comp->func = func;
    comp->pkg = pkg;
    comp->pkgSrcStr = LLVMBuildGlobalStringPtr(builder, pkg->src, "");
    comp->rtlib = rtlib;
}

LLVMValueRef compile(compiler* comp, ast* ast) {
    if(ast == NULL)
        return NULL;

    switch(ast->type) {
        case AST_LITERAL:
            return buildLiteral(comp, (astLiteral*)ast);
        case AST_UNARY:
            return buildUnary(comp, (astUnary*)ast);
        case AST_BINARY:
            return buildBinary(comp, (astBinary*)ast);
        case AST_QUOTE:
            return buildQuote(comp, (astQuote*)ast);
        case AST_EVAL:
            return buildEval(comp, (astEval*)ast);
    }
    return NULL;
}