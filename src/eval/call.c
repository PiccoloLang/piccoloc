
#include "../eval.h"

val evalCall(engine* engine, astCall* call) {
    val fn = eval(engine, call->fn);

    if(!IS_OBJ(fn))
        engine->runtimeError(engine, "Cannot call @.", call->tkn, 1, fn);
    objFn* fnObj = AS_OBJ(fn);
    if(fnObj->obj.type != OBJ_FN)
        engine->runtimeError(engine, "Cannot call @.", call->tkn, 1, fn);

    int argCnt = 0;
    ast* curr = call->args;
    while(curr != NULL) {
        argCnt++;
        curr = curr->next;
    }

    if(argCnt != fnObj->arity)
        engine->runtimeError(engine, "Expected # arguments, got #.", call->tkn, 2, fnObj->arity, argCnt);
    
    val args[argCnt];
    curr = call->args;
    for(int i = 0; i < argCnt; i++) {
        args[i] = eval(engine, curr);
        curr = curr->next;
    }

    return fnObj->fn(args);

}
