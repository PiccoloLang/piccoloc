
#include "builders.h"

#include <stdlib.h>

static LLVMValueRef buildExprQuote(compiler* comp, ast* expr);

static LLVMValueRef buildQuotedUnary(compiler* comp, astUnary* unary) {
    LLVMValueRef val = buildExprQuote(comp, unary->val);
    LLVMValueRef tokenStart = buildStrOffset(comp, comp->pkgSrcStr, buildInt(comp, unary->op.start - comp->pkg->src));
    return buildCall(comp, comp->rtlib->makeQuotedUnary.type, comp->rtlib->makeQuotedUnary.func, 5, 
        val,
        tokenStart,
        buildInt(comp, unary->op.line),
        buildInt(comp, unary->op.len),
        buildInt(comp, unary->op.type)
    );
}

static LLVMValueRef buildQuotedBinary(compiler* comp, astBinary* binary) {
    LLVMValueRef lhs = buildExprQuote(comp, binary->lhs);
    LLVMValueRef rhs = buildExprQuote(comp, binary->rhs);
    LLVMValueRef tokenStart = buildStrOffset(comp, comp->pkgSrcStr, buildInt(comp, binary->op.start - comp->pkg->src));
    return buildCall(comp, comp->rtlib->makeQuotedBinary.type, comp->rtlib->makeQuotedBinary.func, 6,
        lhs,
        rhs, 
        tokenStart,
        buildInt(comp, binary->op.line),
        buildInt(comp, binary->op.len),
        buildInt(comp, binary->op.type)
    );
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

static LLVMValueRef buildQuotedEval(compiler* comp, astEval* eval) {
    LLVMValueRef expr = buildQuote(comp, eval->expr);
    return buildCall(comp, comp->rtlib->makeQuotedEval.type, comp->rtlib->makeQuotedEval.func, 1, expr);
}

static LLVMValueRef buildExprQuote(compiler* comp, ast* expr) {
    switch(expr->type) {
        case AST_LITERAL:
            return buildLiteral(comp, (astLiteral*)expr);
        case AST_UNARY:
            return buildQuotedUnary(comp, (astUnary*)expr);
        case AST_BINARY:
            return buildQuotedBinary(comp, (astBinary*)expr);
        case AST_QUOTE:
            return buildQuotedQuote(comp, (astQuote*)expr);
        case AST_EVAL:
            return buildQuotedEval(comp, (astEval*)expr);
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
