
#include "builders.h"
#include "obj_builders.h"

LLVMValueRef buildFn(compiler* comp, astFn* fnAst) {

    int arity = 0;
    ast* currArg = fnAst->args;
    while(currArg != NULL) {
        arity++;
        currArg = currArg->next;
    }

    LLVMValueRef func = LLVMAddFunction(comp->mod, "fn", comp->types->fnType);
    LLVMBasicBlockRef block = LLVMAppendBasicBlock(func, "");

    LLVMBuilderRef builder = LLVMCreateBuilder();
    LLVMPositionBuilderAtEnd(builder, block);

    compiler innerComp;
    initCompiler(&innerComp, comp->mod, builder, func, comp->pkg, comp->rtlib, comp->types);

    LLVMValueRef argsPtr = LLVMGetParam(func, 0); 

    currArg = fnAst->args;
    while(currArg != NULL) {
        astVar* arg = (astVar*)currArg;
        variable var;
        var.name = arg->name;
        var.idx = innerComp.vars.cnt;
        var.declChrono = fnAst->ast.chrono;
        var.initialized = true;
        var.invalidated = false;
        var.alloc = buildValAlloc(&innerComp);
        buildStore(&innerComp, var.alloc, buildDeref(&innerComp, LLVMBuildGEP2(innerComp.builder, comp->types->valType, argsPtr, (LLVMValueRef[]){LLVMConstInt(LLVMInt32Type(), innerComp.vars.cnt, false)}, 1, ""))); 
        pushDynarr(&innerComp.vars, var); 
        currArg = currArg->next;
    }

    LLVMValueRef returnVal = compile(&innerComp, fnAst->body);
    LLVMBuildRet(builder, returnVal);

    LLVMDisposeBuilder(builder);
    freeCompiler(&innerComp);

    LLVMValueRef objPtr = buildObjAlloc(comp, OBJ_FN);
    LLVMValueRef fnPtr = buildObjFnGetFnPtr(comp, objPtr);
    buildStore(comp, fnPtr, LLVMBuildPointerCast(comp->builder, func, comp->types->fnPtrType, ""));
    LLVMValueRef envPtr = buildObjFnGetRtVarEnvPtr(comp, objPtr);
    buildStore(comp, envPtr, buildCall(comp, comp->rtlib->getCurrRtVar.type, comp->rtlib->getCurrRtVar.func, 0));

    LLVMValueRef arityPtr = buildObjFnGetArityPtr(comp, objPtr);
    buildStore(comp, arityPtr, buildInt(comp, arity));
    
    return buildPtrToVal(comp, objPtr); 
}
