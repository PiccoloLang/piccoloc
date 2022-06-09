
#include "compiler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builders/builders.h"

int findVar(compiler* comp, token tkn) {
    for(int i = comp->vars.cnt - 1; i >= 0; i--)
        if(tkn.len == comp->vars.vals[i].name.len && memcmp(tkn.start, comp->vars.vals[i].name.start, tkn.len) == 0)
            return i;
    return 0;
}

void initCompiler(compiler* comp, LLVMBuilderRef builder, LLVMValueRef func, package* pkg, rtlibFuncs* rtlib) {
    comp->builder = builder;
    comp->func = func;
    comp->pkg = pkg;
    comp->pkgSrcStr = LLVMBuildGlobalStringPtr(builder, pkg->src, "");
    initDynarr(&comp->vars);
    comp->rtlib = rtlib;
}

void freeCompiler(compiler* comp) {
    freeDynarr(&comp->vars);
}

static void compilationError(compiler* comp, token tkn, const char* msg) {
    (void)comp;
    printf("Compilation error lol. Line %d. %s\n", tkn.len, msg);
}

void compileVarDecls(compiler* comp, int firstIdx) {
    for(int i = firstIdx; i < comp->vars.cnt; i++) {
        comp->vars.vals[i].alloc = buildValAlloc(comp);
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
