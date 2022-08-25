
#include "builders.h"
#include "obj_builders.h"
#include "../../ast_util.h"

static LLVMValueRef buildArgArr(compiler* comp, astCall* expr) {
    int argCnt = astNextChainLen(expr->args);
    LLVMValueRef args = LLVMBuildArrayAlloca(comp->builder, comp->types->valType, buildInt(comp, argCnt), "");
    ast* curr = expr->args;
    for(int i = 0; i < argCnt; i++) {
        buildStore(comp, LLVMBuildGEP2(comp->builder, comp->types->valType, args, (LLVMValueRef[]){LLVMConstInt(LLVMInt32Type(), i, false)}, 1, ""), compile(comp, curr));
        curr = curr->next;
    }

    return args;
}

static LLVMValueRef buildCallFn(compiler* comp, astCall* expr, LLVMValueRef fnObjPtr) {

    LLVMValueRef arity = buildDeref(comp, buildObjFnGetArityPtr(comp, fnObjPtr));

    int argCnt = astNextChainLen(expr->args); 

    buildChoiceRes arityCheck = buildChoice(comp, buildIntEq(comp, arity, buildInt(comp, argCnt)));

    compileToBlock(comp, arityCheck.falseCond);
    buildRuntimeError(comp, expr->tkn, "Expected # arguments, got #.", 2, arity, buildInt(comp, argCnt));
    buildJump(comp, arityCheck.trueCond);

    compileToBlock(comp, arityCheck.trueCond);
    LLVMValueRef fnPtr = buildDeref(comp, buildObjFnGetFnPtr(comp, fnObjPtr));

    LLVMValueRef args = buildArgArr(comp, expr);

    LLVMValueRef prevRtVar = buildCall(comp, comp->rtlib->getCurrRtVar.type, comp->rtlib->getCurrRtVar.func, 0);
    LLVMValueRef env = buildDeref(comp, buildObjFnGetRtVarEnvPtr(comp, fnObjPtr));
    buildCall(comp, comp->rtlib->setCurrRtVar.type, comp->rtlib->setCurrRtVar.func, 1, env);
    LLVMValueRef callRet = buildCall(comp, comp->types->fnType, fnPtr, 1, args);
    buildCall(comp, comp->rtlib->setCurrRtVar.type, comp->rtlib->setCurrRtVar.func, 1, prevRtVar);

    return callRet; 
}

static LLVMValueRef buildCallRtFn(compiler* comp, astCall* expr, LLVMValueRef fnObjPtr) {

    LLVMValueRef args = buildArgArr(comp, expr);
    LLVMValueRef fnObjAsVoidPtr = LLVMBuildPointerCast(comp->builder, fnObjPtr, comp->types->voidPtrType, "");
    LLVMValueRef rtFnCallRes = buildCall(comp, comp->rtlib->callRtFn.type, comp->rtlib->callRtFn.func, 2, fnObjAsVoidPtr, args);
    return rtFnCallRes;

}

LLVMValueRef buildCallExpr(compiler* comp, astCall* expr) {

    LLVMValueRef fnVal = compile(comp, expr->fn);
    buildChoiceRes isObjChoice = buildChoice(comp, buildIsObjCheck(comp, fnVal));
    
    compileToBlock(comp, isObjChoice.falseCond);
    buildRuntimeError(comp, expr->tkn, "Cannot call @.", 1, fnVal);
    buildJump(comp, isObjChoice.trueCond);

    compileToBlock(comp, isObjChoice.trueCond);
    LLVMValueRef fnCallRes = buildValAlloc(comp);    
    LLVMBasicBlockRef endBlock = makeBlock(comp);
    LLVMValueRef fnObjPtr = buildValToPtr(comp, fnVal);
    buildChoiceRes isFnChoice = buildChoice(comp, buildIsFnCheck(comp, fnVal));

    compileToBlock(comp, isFnChoice.trueCond);
    buildStore(comp, fnCallRes, buildCallFn(comp, expr, fnObjPtr));
    buildJump(comp, endBlock);

    compileToBlock(comp, isFnChoice.falseCond);
    buildChoiceRes isRtFnChoice = buildChoice(comp, buildIsRtFnCheck(comp, fnVal));

    compileToBlock(comp, isRtFnChoice.trueCond);
    buildStore(comp, fnCallRes, buildCallRtFn(comp, expr, fnObjPtr));
    buildJump(comp, endBlock);
    
    compileToBlock(comp, isRtFnChoice.falseCond);
    buildRuntimeError(comp, expr->tkn, "Cannot call @.", 1, fnVal);
    buildJump(comp, endBlock);

    compileToBlock(comp, endBlock);
    return buildDeref(comp, fnCallRes);
}
