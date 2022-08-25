
#include "../eval.h"
#include "../ast_util.h"

val evalFn(engine* engine, astFn* fn) {
    (void)engine;
    objRtFn* rtFn = ALLOC_OBJ(RtFn, RTFN);
    rtFn->fnAst = (void*)fn;
    rtFn->arity = astNextChainLen(fn->args); 
    return OBJ_VAL(rtFn);
}
