
#include "builders.h"

LLVMValueRef buildVar(compiler* comp, astVar* var) {
    int idx = findVar(comp, var->name);
    if(idx == -1) {
        return buildCall(comp, comp->rtlib->evalRuntimeVariable.type, comp->rtlib->evalRuntimeVariable.func, 2, buildStrOffset(comp, comp->pkgSrcStr, buildInt(comp, var->name.start - comp->pkg->src)), buildInt(comp, var->name.len));
    } else {
        LLVMValueRef val = buildGetPtr(comp, comp->vars.vals[idx].alloc);
        buildChoiceRes choice = buildChoice(comp, buildIsUninitCheck(comp, val));
        compileToBlock(comp, choice.trueCond);
        buildRuntimeError(comp, var->name, "Variable does not exist.", 0);
        buildJump(comp, choice.falseCond);
        compileToBlock(comp, choice.falseCond);
        return val;
    }
}
