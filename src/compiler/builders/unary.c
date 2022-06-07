
#include "builders.h"

#include <stdlib.h>

static LLVMValueRef buildNumericUnary(compiler* comp, astUnary* ast) {
    LLVMValueRef val = compile(comp, ast->val);
    buildChoiceRes cond = buildChoice(comp, buildIsNumCheck(comp, val));

    compileToBlock(comp, cond.falseCond);
    buildRuntimeError(comp, ast->op, ast->op.type == TOKEN_MINUS ? "Cannot negate @." : "A @ is not a number.", 1, val);
    buildJump(comp, cond.trueCond);

    compileToBlock(comp, cond.trueCond);
    if(ast->op.type == TOKEN_PLUS)
        return val;
    else
        return buildNumToVal(comp, buildNumSub(comp, buildDouble(comp, 0), buildValToNum(comp, val)));
}

static LLVMValueRef buildBoolUnary(compiler* comp, astUnary* ast) {
    LLVMValueRef val = compile(comp, ast->val);
    return buildBoolToVal(comp, buildNot(comp, buildTruthy(comp, val)));
}

LLVMValueRef buildUnary(compiler* comp, astUnary* ast) {
    switch(ast->op.type) {
        case TOKEN_PLUS:
        case TOKEN_MINUS:
            return buildNumericUnary(comp, ast);
        case TOKEN_BANG:
            return buildBoolUnary(comp, ast);
        default:
            return NULL;
    }
}
