
#include "builders.h"
#include "../var_find.h"

LLVMValueRef buildBlock(compiler* comp, astBlock* block) {
    int varCntBefore = comp->vars.cnt;
    findVars(comp, block->first);
    compileVarDecls(comp, varCntBefore);

    LLVMValueRef result = buildValue(comp, NIL_VAL());
    ast* curr = block->first;
    while(curr != NULL) {
        result = compile(comp, curr);
        curr = curr->next;
    }

    comp->vars.cnt = varCntBefore;

    return result;
}
