
#include "builders.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

LLVMValueRef buildInt(compiler* comp, int val) {
    (void)comp;
    return LLVMConstInt(LLVMInt32Type(), val, false);
}

LLVMValueRef buildBool(compiler* comp, bool val) {
    (void)comp;
    return LLVMConstInt(LLVMInt1Type(), val, false);
}

LLVMValueRef buildValue(compiler* comp, val val) {
    (void)comp;
    return LLVMConstInt(LLVMInt64Type(), val, false);
}

LLVMValueRef buildValToNum(compiler* comp, LLVMValueRef val) {
    return LLVMBuildBitCast(comp->builder, val, LLVMDoubleType(), "");
}

LLVMValueRef buildNumToVal(compiler* comp, LLVMValueRef val) {
    return LLVMBuildBitCast(comp->builder, val, LLVMInt64Type(), "");
}

LLVMValueRef buildBoolToVal(compiler* comp, LLVMValueRef val) {
    // Since TAG_FALSE is 2 and TAG_TRUE is 3, binary or-ing false with the cond works
    // I <3 NaN boxing :)
    return LLVMBuildOr(comp->builder, 
                LLVMBuildIntCast2(comp->builder, val, LLVMInt64Type(), false, ""),
                buildValue(comp, BOOL_VAL(false)), "");
}



LLVMValueRef buildNumAdd(compiler* comp, LLVMValueRef a, LLVMValueRef b) {
    return LLVMBuildFAdd(comp->builder, a, b, "");
}

LLVMValueRef buildNumSub(compiler* comp, LLVMValueRef a, LLVMValueRef b) {
    return LLVMBuildFSub(comp->builder, a, b, "");
}

LLVMValueRef buildNumMul(compiler* comp, LLVMValueRef a, LLVMValueRef b) {
    return LLVMBuildFMul(comp->builder, a, b, "");
}

LLVMValueRef buildNumDiv(compiler* comp, LLVMValueRef a, LLVMValueRef b) {
    return LLVMBuildFDiv(comp->builder, a, b, "");
}

LLVMValueRef buildNumCmp(compiler* comp, LLVMValueRef a, LLVMValueRef b) {
    return LLVMBuildFCmp(comp->builder, LLVMRealOGT, a, b, "");
}

LLVMValueRef buildDouble(compiler* comp, double val) {
    (void)comp;
    return LLVMConstReal(LLVMDoubleType(), val);
}



LLVMValueRef buildString(compiler* comp, const char* s) {
    return LLVMBuildGlobalStringPtr(comp->builder, s, "");
}



LLVMValueRef buildAnd(compiler* comp, LLVMValueRef a, LLVMValueRef b) {
    return LLVMBuildAnd(comp->builder, a, b, "");
}

LLVMValueRef buildNot(compiler* comp, LLVMValueRef val) {
    return LLVMBuildNot(comp->builder, val, "");
}



LLVMBasicBlockRef makeBlock(compiler* comp) {
    return LLVMAppendBasicBlock(comp->func, "");
}

void buildJump(compiler* comp, LLVMBasicBlockRef block) {
    LLVMBuildBr(comp->builder, block);
}

buildChoiceRes buildChoice(compiler* comp, LLVMValueRef cond) {
    LLVMBasicBlockRef trueCond = LLVMAppendBasicBlock(comp->func, "");
    LLVMBasicBlockRef falseCond = LLVMAppendBasicBlock(comp->func, "");
    LLVMBuildCondBr(comp->builder, cond, trueCond, falseCond);
    buildChoiceRes res;
    res.trueCond = trueCond;
    res.falseCond = falseCond;
    return res;
}



LLVMValueRef buildValAlloc(compiler* comp) {
    return LLVMBuildAlloca(comp->builder, LLVMInt64Type(), "");
}

LLVMValueRef buildGetPtr(compiler* comp, LLVMValueRef ptr) {
    return LLVMBuildLoad2(comp->builder, LLVMInt64Type(), ptr, "");
}

LLVMValueRef buildSetPtr(compiler* comp, LLVMValueRef ptr, LLVMValueRef val) {
    return LLVMBuildStore(comp->builder, val, ptr);
}



void compileToBlock(compiler* comp, LLVMBasicBlockRef block) {
    LLVMPositionBuilderAtEnd(comp->builder, block);
}



LLVMValueRef buildIsNilCheck(compiler* comp, LLVMValueRef val) {
    LLVMValueRef nil = LLVMBuildICmp(comp->builder, LLVMIntEQ, val, buildValue(comp, NIL_VAL()), "");
    return nil;
}

LLVMValueRef buildIsNumCheck(compiler* comp, LLVMValueRef val) {
    LLVMValueRef bitwiseAndResult = LLVMBuildAnd(comp->builder, val, buildValue(comp, QNAN), ""); // val & QNAN
    LLVMValueRef result = LLVMBuildICmp(comp->builder, LLVMIntNE, bitwiseAndResult, buildValue(comp, QNAN), ""); // (val & QNAN) != QNAN
    return result;
}

LLVMValueRef buildIsBoolCheck(compiler* comp, LLVMValueRef val) {
    LLVMValueRef bitwiseOrResult = LLVMBuildOr(comp->builder, val, buildValue(comp, 1), ""); // val | 1
    LLVMValueRef result = LLVMBuildICmp(comp->builder, LLVMIntEQ, bitwiseOrResult, buildValue(comp, BOOL_VAL(true)), ""); // (val | 1) == BOOL_VAL(true)
    return result;
}

LLVMValueRef buildTruthy(compiler* comp, LLVMValueRef val) {
    LLVMValueRef notNil = LLVMBuildNot(comp->builder, buildIsNilCheck(comp, val), "");
    LLVMValueRef notFalse = LLVMBuildICmp(comp->builder, LLVMIntNE, val, buildValue(comp, BOOL_VAL(false)), "");
    return buildAnd(comp, notNil, notFalse);
}



LLVMValueRef buildCall(compiler* comp, LLVMTypeRef type, LLVMValueRef func, int argc, ...) {

    va_list args;
    va_start(args, argc);
    LLVMValueRef fnArgs[argc];
    for(int i = 0; i < argc; i++)
        fnArgs[i] = va_arg(args, LLVMValueRef);
    va_end(args);

    return LLVMBuildCall2(comp->builder, type, func, fnArgs, argc, "");
}

LLVMValueRef buildCallFromArgArray(compiler* comp, LLVMTypeRef type, LLVMValueRef func, int argc, LLVMValueRef* args) {
    return LLVMBuildCall2(comp->builder, type, func, args, argc, "");
}



void buildRuntimeError(compiler* comp, token tkn, const char* msg, int argc, ...) {

    const char* lineStart = tkn.start;
    while(lineStart != comp->pkg->src && *lineStart != '\n')
        lineStart--;
    if(*lineStart == '\n')
        lineStart++;
    
    const char* lineEnd = lineStart;
    while(*lineEnd != '\n' && *lineEnd != '\0')
        lineEnd++;
    
    char* line = malloc(lineEnd - lineStart + 1);
    memcpy(line, lineStart, lineEnd - lineStart);
    line[lineEnd - lineStart] = '\0';

    LLVMValueRef args[argc + 5];
    args[0] = buildInt(comp, tkn.line);
    args[1] = buildInt(comp, tkn.start - lineStart);
    args[2] = LLVMBuildGlobalStringPtr(comp->builder, line, "");
    args[3] = LLVMBuildGlobalStringPtr(comp->builder, msg, "");
    args[4] = buildInt(comp, argc);

    va_list argList;
    va_start(argList, argc);
    for(int i = 0; i < argc; i++)
        args[5 + i] = va_arg(argList, LLVMValueRef);
    va_end(argList);
    
    LLVMBuildCall2(comp->builder, comp->rtlib->runtimeError.type, comp->rtlib->runtimeError.func, args, argc + 5, "");

}



LLVMValueRef buildStrOffset(compiler* comp, LLVMValueRef str, LLVMValueRef offset) {
    return LLVMBuildIntToPtr(comp->builder,
        LLVMBuildAdd(comp->builder, LLVMBuildPtrToInt(comp->builder, str, LLVMInt64Type(), ""), offset, ""),
        LLVMPointerType(LLVMInt8Type(), 0), "");

}
