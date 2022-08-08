
#include "builders.h"
#include "../var_find.h"

LLVMValueRef buildBlock(compiler* comp, astBlock* block) {

    int oldOuterScopeVars = comp->outerScopeVars;
    comp->outerScopeVars = comp->vars.cnt;

    int varCntBefore = comp->vars.cnt;

    bool varValidation[varCntBefore];
    for(int i = 0; i < varCntBefore; i++)
        varValidation[i] = comp->vars.vals[i].invalidated;
    
    findVars(comp, block->first);
    compileVarDecls(comp, varCntBefore);

    LLVMValueRef oldRtVarPtr = buildPtrAlloc(comp);
    buildSetPtr(comp, oldRtVarPtr, buildCall(comp, comp->rtlib->getCurrRtVar.type, comp->rtlib->getCurrRtVar.func, 0));

    LLVMValueRef result = buildValue(comp, NIL_VAL());
    ast* curr = block->first;
    while(curr != NULL) {
        result = compile(comp, curr);
        curr = curr->next;
    }

    comp->vars.cnt = varCntBefore;

    buildCall(comp, comp->rtlib->returnToRtVar.type, comp->rtlib->returnToRtVar.func, 1, buildGetPtrPtr(comp, oldRtVarPtr));

    comp->outerScopeVars = oldOuterScopeVars;

    for(int i = 0; i < varCntBefore; i++)
        comp->vars.vals[i].invalidated = varValidation[i];

    return result;
}
