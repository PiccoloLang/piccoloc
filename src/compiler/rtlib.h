
#ifndef RTLIB_H
#define RTLIB_H

#include <llvm-c/Core.h>

#define TYPE_VOID LLVMVoidType()
#define TYPE_INT LLVMInt32Type()
#define TYPE_BOOL LLVMInt1Type()
#define TYPE_STR LLVMPointerType(LLVMInt8Type(), 0)
#define TYPE_VAL LLVMInt64Type()
#define TYPE_VAL_PTR LLVMPointerType(TYPE_VAL, 0)
#define TYPE_VOID_PTR LLVMPointerType(TYPE_VOID, 0)

#define RTLIB_FUNC_X \
    RTLIB_FUNC(initRTLib, TYPE_VOID, 1, false, TYPE_STR) \
    RTLIB_FUNC(runtimeError, TYPE_VOID, 5, true, TYPE_INT, TYPE_INT, TYPE_STR, TYPE_STR, TYPE_INT) \
    RTLIB_FUNC(makeString, TYPE_VAL, 1, false, TYPE_STR) \
    RTLIB_FUNC(add, TYPE_VAL, 2, false, TYPE_VAL, TYPE_VAL) \
    \
    RTLIB_FUNC(makeQuotedVar, TYPE_VAL, 4, false, TYPE_STR, TYPE_INT, TYPE_INT, TYPE_INT) \
    RTLIB_FUNC(makeQuotedUnary, TYPE_VAL, 5, false, TYPE_VAL, TYPE_STR, TYPE_INT, TYPE_INT, TYPE_INT) \
    RTLIB_FUNC(makeQuotedBinary, TYPE_VAL, 6, false, TYPE_VAL, TYPE_VAL, TYPE_STR, TYPE_INT, TYPE_INT, TYPE_INT) \
    RTLIB_FUNC(makeQuotedBlock, TYPE_VAL, 1, true, TYPE_INT) \
    RTLIB_FUNC(makeQuotedQuote, TYPE_VAL, 1, true, TYPE_INT) \
    RTLIB_FUNC(makeQuotedEval, TYPE_VAL, 1, false, TYPE_VAL) \
    RTLIB_FUNC(makeQuotedInquote, TYPE_VAL, 1, false, TYPE_VAL) \
    RTLIB_FUNC(makeQuotedVarDecl, TYPE_VAL, 6, false, TYPE_STR, TYPE_INT, TYPE_INT, TYPE_INT, TYPE_VAL, TYPE_BOOL) \
    \
    RTLIB_FUNC(evaluateQuote, TYPE_VAL, 1, false, TYPE_VAL) \
    \
    RTLIB_FUNC(evalRuntimeVariable, TYPE_VAL, 2, false, TYPE_STR, TYPE_INT) \
    RTLIB_FUNC(getCurrRtVar, TYPE_VOID_PTR, 0, false, TYPE_VOID) \
    RTLIB_FUNC(returnToRtVar, TYPE_VOID, 1, false, TYPE_VOID_PTR) \
    RTLIB_FUNC(addRtVar, TYPE_VOID, 3, false, TYPE_STR, TYPE_INT, TYPE_VAL_PTR)

typedef struct {
    LLVMValueRef func;
    LLVMTypeRef type;
} rtlibFunc;

typedef struct {
#define RTLIB_FUNC(name, retType, argCnt, vararg, ...) rtlibFunc name;
    RTLIB_FUNC_X
#undef RTLIB_FUNC
} rtlibFuncs;

void initRTlib(rtlibFuncs* rtlib, LLVMModuleRef mod);

#endif
