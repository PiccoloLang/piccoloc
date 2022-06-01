
#include "../value.h"
#include "../obj.h"

val evaluateQuote(val expr) {
    if(IS_OBJ(expr)) {
        obj* o = AS_OBJ(expr);
        if(o->type == OBJ_QUOTE) {
            // TODO: make AST walk evaluator here
        }
    }
    return expr;
}