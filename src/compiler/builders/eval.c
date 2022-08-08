
#include "builders.h"

LLVMValueRef buildEval(compiler* comp, astEval* ast) {
    LLVMValueRef expr = compile(comp, ast->expr);

    for(int i = 0; i < comp->vars.cnt; i++) {
        if(comp->vars.vals[i].declChrono < comp->prevEvalChrono || !comp->vars.vals[i].initialized)
            continue;
        variable* var = comp->vars.vals + i;
        buildCall(comp, comp->rtlib->addRtVar.type, comp->rtlib->addRtVar.func, 3, buildStrOffset(comp, comp->pkgSrcStr, buildInt(comp, var->name.start - comp->pkg->src)), buildInt(comp, var->name.len), var->alloc);
    }

    for(int i = 0; i < comp->vars.cnt; i++)
        comp->vars.vals[i].invalidated = true;
    
    return buildCall(comp, comp->rtlib->evaluateQuote.type, comp->rtlib->evaluateQuote.func, 1, expr);
}
