
#include "compiler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "builders/builders.h"

// Finds the index of a variable, or returns -1 if it isn't declared
int findVar(compiler* comp, token tkn) {
    int bestUninitialized = -1;
    for(int i = comp->vars.cnt - 1; i >= 0; i--) {
        if(tkn.len == comp->vars.vals[i].name.len && memcmp(tkn.start, comp->vars.vals[i].name.start, tkn.len) == 0) {
            if(comp->vars.vals[i].invalidated)
                continue;
            if(comp->vars.vals[i].initialized)
                return i;
            if(bestUninitialized == -1)
                bestUninitialized = i;
        }
    }
    return bestUninitialized;
}

void initCompiler(compiler* comp, LLVMBuilderRef builder, LLVMValueRef func, package* pkg, rtlibFuncs* rtlib) {
    comp->builder = builder;
    comp->func = func;
    comp->pkg = pkg;
    comp->pkgSrcStr = LLVMBuildGlobalStringPtr(builder, pkg->src, "");
    initDynarr(&comp->vars);
    comp->rtlib = rtlib;
    comp->hadError = false;
    comp->prevEvalChrono = 0;
    comp->outerScopeVars = 0;
}

void freeCompiler(compiler* comp) {
    freeDynarr(&comp->vars);
}

void compilationError(compiler* comp, token tkn, const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    char buff[512];
    vsnprintf(buff, 512, msg, args);
    va_end(args);
    printf("Compilation error. Line %d. %s\n", tkn.line, buff);
    comp->hadError = true;
}

void compileVarDecls(compiler* comp, int firstIdx) {
    for(int i = firstIdx; i < comp->vars.cnt; i++) {
        comp->vars.vals[i].alloc = buildValAlloc(comp);
        buildSetPtr(comp, comp->vars.vals[i].alloc, buildValue(comp, UNINIT_VAL()));
    }
}

LLVMValueRef compile(compiler* comp, ast* ast) {
    if(ast == NULL)
        return NULL;

    switch(ast->type) {
        case AST_LITERAL:
            return buildLiteral(comp, (astLiteral*)ast);
        case AST_VAR:
            return buildVar(comp, (astVar*)ast);
        case AST_UNARY:
            return buildUnary(comp, (astUnary*)ast);
        case AST_BINARY:
            return buildBinary(comp, (astBinary*)ast);
        case AST_BLOCK:
            return buildBlock(comp, (astBlock*)ast);
        case AST_QUOTE:
            return buildQuote(comp, (astQuote*)ast);
        case AST_EVAL:
            return buildEval(comp, (astEval*)ast);
        case AST_INQUOTE: {
            astInquote* inquote = (astInquote*)ast;
            compilationError(comp, inquote->op, "Cannot inquote outside quote.");
            return NULL;
        }
        case AST_VARDECL:
            return buildVarDecl(comp, (astVarDecl*)ast);
    }
}
