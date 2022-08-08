
#include "../eval.h"

val evalBlock(engine* engine, astBlock* block) {

    rtVariable* prevVars = engine->currVar;

    val res = NIL_VAL();
    ast* curr = block->first;
    while(curr != NULL) {
        res = eval(engine, curr);
        curr = curr->next;
    }

    returnToVar(engine, prevVars);

    return res;
}
