
#include "rtlib.h"

#include <stdbool.h>
#include <stdarg.h>

static LLVMTypeRef makeFuncType(LLVMTypeRef retType, bool vararg, int argCnt, ...) {

    LLVMTypeRef argTypes[argCnt];

    va_list args;
    va_start(args, argCnt);
    for(int i = 0; i < argCnt; i++)
        argTypes[i] = va_arg(args, LLVMTypeRef);
    va_end(args);

    return LLVMFunctionType(retType, argTypes, argCnt, vararg);
}

void initRTlib(rtlibFuncs* rtlib, LLVMModuleRef mod, types* t) {

    #define RTLIB_FUNC(name, retType, argCnt, vararg, ...) \
        rtlib->name.type = makeFuncType(retType, vararg, argCnt, __VA_ARGS__); \
        rtlib->name.func = LLVMAddFunction(mod, #name, rtlib->name.type);
    
    LLVMTypeRef argTypes[] = {LLVMInt32Type(), LLVMPointerType(LLVMInt64Type(), 0)};
    LLVMTypeRef fnPtrType = LLVMPointerType(LLVMFunctionType(LLVMInt64Type(), argTypes, 2, false), 0);

    RTLIB_FUNC_X
}
