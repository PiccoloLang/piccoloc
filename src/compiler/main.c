
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "../package.h"
#include "parser.h"
#include "compiler.h"
#include "../debug.h"
#include "builders/builders.h"

#include <stdio.h>

#include <llvm-c/Core.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>

char* concat(const char* s1, const char* s2) {
    size_t l1 = strlen(s1);
    size_t l2 = strlen(s2);
    char* res = malloc(l1 + l2 + 1);
    memcpy(res, s1, l1);
    memcpy(res + l1, s2, l2);
    res[l1 + l2] = '\0';
    return res;
}

char* multiconcat(int n, ...) {
    va_list list;
    va_start(list, n);

    char* result = NULL;
    size_t currSize = 0;
    for(int i = 0; i < n; i++) { 
        const char* str = va_arg(list, const char*);
        size_t len = strlen(str);
        result = realloc(result, currSize + len + 1);
        memcpy(result + currSize, str, len);
        currSize += len;
    }
    result[currSize] = '\0';

    va_end(list);

    return result;
}

int main(int argc, const char** argv) { // TODO: clean up this *absolute mess*, make cross platform
    const char* homePath = getenv("HOME");
    char* filePath = concat(homePath, "/Library/Application Support/piccolo/");
    char* shellFilePath = concat(homePath, "/Library/Application\\ Support/piccolo/");

    if(argc != 2) {
        printf("Usage: piccoloc [path/to/code.pic]\n");
        return -1;
    }

    package pkg;
    if(!loadPackage(&pkg, argv[1])) {
        printf("Could not load package '%s'.\n", argv[1]);
        return -1;
    }

    LLVMModuleRef mod = LLVMModuleCreateWithName("module");
    
    LLVMTypeRef fnType = LLVMFunctionType(LLVMVoidType(), NULL, 0, false);
    LLVMValueRef mainFn = LLVMAddFunction(mod, "main", fnType);
    LLVMBasicBlockRef block = LLVMAppendBasicBlock(mainFn, "");
    LLVMBuilderRef builder = LLVMCreateBuilder();

    LLVMTypeRef printParamTypes[] = {LLVMInt64Type()};
    LLVMTypeRef printFnType = LLVMFunctionType(LLVMVoidType(), printParamTypes, 1, false);
    LLVMValueRef printFn = LLVMAddFunction(mod, "print", printFnType);

    LLVMPositionBuilderAtEnd(builder, block);

    rtlibFuncs rtlib;
    initRTlib(&rtlib, mod);

    compiler comp;
    initCompiler(&comp, builder, mainFn, &pkg, &rtlib);

    parser prsr;
    initParser(&prsr, pkg.src);

    buildCall(&comp, comp.rtlib->initRTLib.type, comp.rtlib->initRTLib.func, 1, buildString(&comp, pkg.src));
    
    while(!parserEOF(&prsr)) {
        ast* ast = parse(&prsr);
        // dumpAst(ast);
        if(!prsr.hadError) {
            LLVMValueRef val = compile(&comp, ast);
            LLVMValueRef args[] = {val};
            LLVMBuildCall2(builder, printFnType, printFn, args, 1, "");
            prsr.hadError = false;
        }
    }

    freeParser(&prsr);

    LLVMBuildRet(builder, NULL);

    freePackage(&pkg);

    char* errMsg = NULL;
    LLVMVerifyModule(mod, LLVMAbortProcessAction, &errMsg);
    LLVMDisposeMessage(errMsg);

    char* bitcodeFilePath = concat(filePath, "module.bc");
    remove(bitcodeFilePath);
    LLVMWriteBitcodeToFile(mod, bitcodeFilePath);
    free(bitcodeFilePath);

    remove("module");
    char* command = multiconcat(5, "llvm-gcc ", shellFilePath, "module.bc ", shellFilePath, "rtlib/libpiccrtlib.a -o module");
    system(command);
    free(command);

    free(filePath);
    free(shellFilePath);

    return 0;
}
