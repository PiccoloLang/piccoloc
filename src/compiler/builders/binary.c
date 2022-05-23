
#include "builders.h"

#include <stdlib.h>

#define BINARY_PREAMBLE \
    LLVMValueRef lhs = compile(comp, ast->lhs); \
    LLVMValueRef rhs = compile(comp, ast->rhs); \
    LLVMValueRef resultPtr = buildValAlloc(comp); \
    LLVMBasicBlockRef endBlock = makeBlock(comp);

#define NUMBER_OP_PREAMBLE \
    BINARY_PREAMBLE \
    LLVMValueRef lhsIsNum = buildIsNumCheck(comp, lhs); \
    LLVMValueRef rhsIsNum = buildIsNumCheck(comp, rhs); \
    LLVMValueRef bothNum = buildAnd(comp, lhsIsNum, rhsIsNum); \
    LLVMValueRef lhsAsNum = buildValToNum(comp, lhs); \
    LLVMValueRef rhsAsNum = buildValToNum(comp, rhs); \
    buildChoiceRes numChoice = buildChoice(comp, bothNum);

static LLVMValueRef buildAdd(compiler* comp, astBinary* ast) { // TODO: make this work with whatever LLVMBuildPHI is
    NUMBER_OP_PREAMBLE    

    compileToBlock(comp, numChoice.trueCond);
    buildSetPtr(comp, resultPtr, 
        buildNumToVal(comp, buildNumAdd(comp, lhsAsNum, rhsAsNum)));
    buildJump(comp, endBlock);

    compileToBlock(comp, numChoice.falseCond);
    buildSetPtr(comp, resultPtr, buildCall(comp, comp->rtlib->add.type, comp->rtlib->add.func, 2, lhs, rhs));
    buildJump(comp, endBlock);

    compileToBlock(comp, endBlock);
    return buildGetPtr(comp, resultPtr);
}

static LLVMValueRef buildSub(compiler* comp, astBinary* ast) {
    NUMBER_OP_PREAMBLE

    compileToBlock(comp, numChoice.trueCond);
    buildSetPtr(comp, resultPtr,
        buildNumToVal(comp, buildNumSub(comp, lhsAsNum, rhsAsNum)));
    buildJump(comp, endBlock);
    
    compileToBlock(comp, numChoice.falseCond);
    buildRuntimeError(comp, ast->op, "Cannot subtract @ from @.", 2, rhs, lhs);
    buildJump(comp, endBlock);

    compileToBlock(comp, endBlock);
    return buildGetPtr(comp, resultPtr);
}

static LLVMValueRef buildMul(compiler* comp, astBinary* ast) {
    NUMBER_OP_PREAMBLE

    compileToBlock(comp, numChoice.trueCond);
    buildSetPtr(comp, resultPtr,
        buildNumToVal(comp, buildNumMul(comp, lhsAsNum, rhsAsNum)));
    buildJump(comp, endBlock);
    
    compileToBlock(comp, numChoice.falseCond);
    buildRuntimeError(comp, ast->op, "Cannot multiply @ by @.", 2, lhs, rhs); // TODO: make str mul
    buildJump(comp, endBlock);

    compileToBlock(comp, endBlock);
    return buildGetPtr(comp, resultPtr);
}

static LLVMValueRef buildDiv(compiler* comp, astBinary* ast) {
    NUMBER_OP_PREAMBLE

    compileToBlock(comp, numChoice.trueCond);
    buildSetPtr(comp, resultPtr,
        buildNumToVal(comp, buildNumDiv(comp, lhsAsNum, rhsAsNum)));
    buildJump(comp, endBlock);
    
    compileToBlock(comp, numChoice.falseCond);
    buildRuntimeError(comp, ast->op, "Cannot divide @ by @.", 2, lhs, rhs);
    buildJump(comp, endBlock);

    compileToBlock(comp, endBlock);
    return buildGetPtr(comp, resultPtr);
}

LLVMValueRef buildAndOp(compiler* comp, astBinary* ast) {
    BINARY_PREAMBLE
    
    buildChoiceRes truthyChoice = buildChoice(comp, buildTruthy(comp, lhs));
    
    compileToBlock(comp, truthyChoice.trueCond);
    buildSetPtr(comp, resultPtr, rhs);
    buildJump(comp, endBlock);

    compileToBlock(comp, truthyChoice.falseCond);
    buildSetPtr(comp, resultPtr, lhs);
    buildJump(comp, endBlock);

    compileToBlock(comp, endBlock);
    return buildGetPtr(comp, resultPtr);
}

LLVMValueRef buildOrOp(compiler* comp, astBinary* ast) {
    BINARY_PREAMBLE
    
    buildChoiceRes truthyChoice = buildChoice(comp, buildTruthy(comp, lhs));
    
    compileToBlock(comp, truthyChoice.trueCond);
    buildSetPtr(comp, resultPtr, lhs);
    buildJump(comp, endBlock);

    compileToBlock(comp, truthyChoice.falseCond);
    buildSetPtr(comp, resultPtr, rhs);
    buildJump(comp, endBlock);

    compileToBlock(comp, endBlock);
    return buildGetPtr(comp, resultPtr);
}

LLVMValueRef buildComparison(compiler* comp, astBinary* ast) {
    NUMBER_OP_PREAMBLE

    compileToBlock(comp, numChoice.trueCond);
    LLVMValueRef result = NULL;
    if(ast->op.type == TOKEN_GREATER)
        result = buildNumCmp(comp, lhsAsNum, rhsAsNum);
    else if(ast->op.type == TOKEN_LESS)
        result = buildNumCmp(comp, rhsAsNum, lhsAsNum);
    else if(ast->op.type == TOKEN_GREATER_EQ)
        result = buildNot(comp, buildNumCmp(comp, rhsAsNum, lhsAsNum));
    else if(ast->op.type == TOKEN_LESS_EQ)
        result = buildNot(comp, buildNumCmp(comp, lhsAsNum, rhsAsNum));
    buildSetPtr(comp, resultPtr, buildBoolToVal(comp, result));
    buildJump(comp, endBlock);

    compileToBlock(comp, numChoice.falseCond);
    buildRuntimeError(comp, ast->op, "Cannot compare @ and @.", 2, lhs, rhs);
    buildJump(comp, endBlock);

    compileToBlock(comp, endBlock);
    return buildGetPtr(comp, resultPtr);
}

LLVMValueRef buildBinary(compiler* comp, astBinary* ast) {
    switch(ast->op.type) {
        case TOKEN_PLUS:
            return buildAdd(comp, ast);
        case TOKEN_MINUS:
            return buildSub(comp, ast);
        case TOKEN_STAR:
            return buildMul(comp, ast);
        case TOKEN_SLASH:
            return buildDiv(comp, ast);
        case TOKEN_AND:
            return buildAndOp(comp, ast);
        case TOKEN_OR:
            return buildOrOp(comp, ast);
        case TOKEN_GREATER:
        case TOKEN_LESS:
        case TOKEN_GREATER_EQ:
        case TOKEN_LESS_EQ:
            return buildComparison(comp, ast);
        default:
            return NULL;
    }
}