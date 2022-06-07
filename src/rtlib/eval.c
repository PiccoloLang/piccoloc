
#include "../value.h"
#include "../obj.h"
#include "../eval.h"

#include "rtlib.h"

#include <stdlib.h>

val evaluateQuote(val expr) {
    if(IS_OBJ(expr)) {
        obj* o = AS_OBJ(expr);
        if(o->type == OBJ_QUOTE) {
            ast* expr = (ast*)o;
            return eval(&rtEngine, expr);
        }
    }
    return expr;
}
