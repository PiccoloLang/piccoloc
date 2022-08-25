
#include "builders.h"

#include <stdlib.h>

static LLVMValueRef buildExprQuote(compiler* comp, ast* expr);

#include "quote_gen.c"
#include "../../ast_util.h"

static LLVMValueRef buildQuotedBlock(compiler* comp, astBlock* block) {
    int exprCnt = 0;
    ast* curr = block->first;
    while(curr != NULL) {
        exprCnt++;
        curr = curr->next;
    }
    LLVMValueRef args[exprCnt + 1];
    args[0] = buildInt(comp, exprCnt);
    curr = block->first;
    for(int i = 1; i <= exprCnt; i++) {
        args[i] = buildExprQuote(comp, curr);
        curr = curr->next;
    }
    return buildCallFromArgArray(comp, comp->rtlib->makeQuotedBlock.type, comp->rtlib->makeQuotedBlock.func, exprCnt + 1, args);
}

static LLVMValueRef buildQuotedCall(compiler* comp, astCall* call) {
    
    int argCnt = 0;
    ast* curr = call->args;
    while(curr != NULL) {
        argCnt++;
        curr = curr->next;
    }

    LLVMValueRef args[argCnt + 5];
    args[0] = buildExprQuote(comp, call->fn);
    args[1] = buildStrOffset(comp, comp->pkgSrcStr, buildInt(comp, call->tkn.start - comp->pkg->src));
    args[2] = buildInt(comp, call->tkn.line);
    args[3] = buildInt(comp, call->tkn.len);
    args[4] = buildInt(comp, call->tkn.type);
    args[5] = buildInt(comp, argCnt);

    curr = call->args;
    for(int i = 0; i < argCnt; i++) {
        args[6 + i] = buildExprQuote(comp, curr);
        curr = curr->next;
    }

    return buildCallFromArgArray(comp, comp->rtlib->makeQuotedCall.type, comp->rtlib->makeQuotedCall.func, argCnt + 6, args);
}

static LLVMValueRef buildQuotedQuote(compiler* comp, astQuote* quote) {
    int exprCnt = 0;
    ast* curr = quote->first;
    while(curr != NULL) {
        exprCnt++;
        curr = curr->next;
    }
    LLVMValueRef args[exprCnt + 1];
    args[0] = buildInt(comp, exprCnt);
    curr = quote->first;
    for(int i = 1; i <= exprCnt; i++) {
        args[i] = buildExprQuote(comp, curr);
        curr = curr->next;
    }
    return buildCallFromArgArray(comp, comp->rtlib->makeQuotedQuote.type, comp->rtlib->makeQuotedQuote.func, exprCnt + 1, args);
}

static LLVMValueRef buildQuotedFn(compiler* comp, astFn* fn) {
    
    int argCnt = astNextChainLen(fn->args);

    LLVMValueRef args[argCnt + 2];
    args[0] = buildExprQuote(comp, fn->body);
    args[1] = buildInt(comp, argCnt);

    ast* curr = fn->args;
    for(int i = 0; i < argCnt; i++) {
        args[i + 2] = buildExprQuote(comp, curr);
        curr = curr->next;
    } 

    return buildCallFromArgArray(comp, comp->rtlib->makeQuotedFn.type, comp->rtlib->makeQuotedFn.func, argCnt + 2, args);

}

static LLVMValueRef buildExprQuote(compiler* comp, ast* expr) {
    switch(expr->type) {
        case AST_LITERAL:
            return buildLiteral(comp, (astLiteral*)expr);
        case AST_VAR:
            return buildQuotedVar(comp, (astVar*)expr);
        case AST_UNARY:
            return buildQuotedUnary(comp, (astUnary*)expr);
        case AST_BLOCK:
            return buildQuotedBlock(comp, (astBlock*)expr);
        case AST_CALL:
            return buildQuotedCall(comp, (astCall*)expr);
        case AST_BINARY:
            return buildQuotedBinary(comp, (astBinary*)expr);
        case AST_QUOTE:
            return buildQuotedQuote(comp, (astQuote*)expr);
        case AST_EVAL:
            return buildQuotedEval(comp, (astEval*)expr);
        case AST_INQUOTE:
            return compile(comp, ((astInquote*)expr)->expr);
        case AST_VARDECL:
            return buildQuotedVarDecl(comp, (astVarDecl*)expr);
        case AST_FN:
            return buildQuotedFn(comp, (astFn*)expr);
    }
}

LLVMValueRef buildQuote(compiler* comp, astQuote* quote) {
    if(quote->first == NULL) {
        return buildValue(comp, NIL_VAL());
    }
    ast* curr = quote->first;
    int exprCnt = 0;
    while(curr != NULL) {
        curr = curr->next;
        exprCnt++;
    }
    if(exprCnt == 1) {
        return buildExprQuote(comp, quote->first);
    } else {
        // TODO: once i add arrays come back and make this work
    }
    return NULL;
}
