
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

void initCompiler(compiler* comp, LLVMModuleRef mod, LLVMBuilderRef builder, LLVMValueRef func, package* pkg, rtlibFuncs* rtlib, types* types) {
    comp->mod = mod;
    comp->builder = builder;
    comp->func = func;
    comp->pkg = pkg;
    comp->pkgSrcStr = LLVMBuildGlobalStringPtr(builder, pkg->src, "");
    initDynarr(&comp->vars);
    comp->hadError = false;
    comp->prevEvalChrono = 0;
    comp->outerScopeVars = 0;

    comp->rtlib = rtlib;
    comp->types = types;

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
        case AST_CALL: 
            return buildCallExpr(comp, (astCall*)ast);
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
        case AST_FN: 
            return buildFn(comp, (astFn*)ast);
    }
}

void insertDynvarConversion(compiler* comp, ast* expr) {
    (void)expr;
    for(int i = 0; i < comp->vars.cnt; i++) {
        if(comp->vars.vals[i].declChrono < comp->prevEvalChrono || !comp->vars.vals[i].initialized)
            continue;
        variable* var = comp->vars.vals + i;
        buildCall(comp, comp->rtlib->addRtVar.type, comp->rtlib->addRtVar.func, 3, buildStrOffset(comp, comp->pkgSrcStr, buildInt(comp, var->name.start - comp->pkg->src)), buildInt(comp, var->name.len), var->alloc);
    }

    for(int i = 0; i < comp->vars.cnt; i++)
        comp->vars.vals[i].invalidated = true;

}
