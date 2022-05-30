
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

static LLVMValueRef buildExprQuote(compiler* comp, ast* expr) {
    switch(expr->type) {
        case AST_LITERAL:
            return buildLiteral(comp, (astLiteral*)expr);
        case AST_UNARY:
            return buildQuotedUnary(comp, (astUnary*)expr);
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
