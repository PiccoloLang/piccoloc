
#include "../eval.h"

val evalBlock(engine* engine, astBlock* block) {
    val res = NIL_VAL();
    ast* curr = block->first;
    while(curr != NULL) {
        res = eval(engine, curr);
        curr = curr->next;
    }
    return res;
}
