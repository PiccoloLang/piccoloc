
#ifndef RTLIB_H
#define RTLIB_H

#include <llvm-c/Core.h>

#define TYPE_VOID LLVMVoidType()
#define TYPE_INT LLVMInt32Type()
#define TYPE_STR LLVMPointerType(LLVMInt8Type(), 0)
#define TYPE_VAL LLVMInt64Type()

#define RTLIB_FUNC_X \
    RTLIB_FUNC(initRTLib, TYPE_VOID, 1, false, TYPE_STR) \
    RTLIB_FUNC(runtimeError, TYPE_VOID, 5, true, TYPE_INT, TYPE_INT, TYPE_STR, TYPE_STR, TYPE_INT) \
    RTLIB_FUNC(makeString, TYPE_VAL, 1, false, TYPE_STR) \
    RTLIB_FUNC(add, TYPE_VAL, 2, false, TYPE_VAL, TYPE_VAL) \
    \
    RTLIB_FUNC(makeQuotedUnary, TYPE_VAL, 5, false, TYPE_VAL, TYPE_STR, TYPE_INT, TYPE_INT, TYPE_INT) \
    RTLIB_FUNC(makeQuotedBinary, TYPE_VAL, 6, false, TYPE_VAL, TYPE_VAL, TYPE_STR, TYPE_INT, TYPE_INT, TYPE_INT) \
    RTLIB_FUNC(makeQuotedQuote, TYPE_VAL, 1, true, TYPE_INT) \
    RTLIB_FUNC(makeQuotedEval, TYPE_VAL, 1, false, TYPE_VAL) \
    \
    RTLIB_FUNC(evaluateQuote, TYPE_VAL, 1, false, TYPE_VAL)

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