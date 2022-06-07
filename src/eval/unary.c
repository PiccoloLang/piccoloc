
#include "../eval.h"

val evalUnary(engine* engine, astUnary* unary) {
    val x = eval(engine, unary->val);
    switch(unary->op.type) {
        case TOKEN_PLUS: {
            if(!IS_NUM(x)) {
                engine->runtimeError(engine, "@ is not a number.", unary->op, 1, x);
            }
            return x;
        }
        case TOKEN_MINUS: {
            if(!IS_NUM(x)) {
                engine->runtimeError(engine, "Cannot negate @.", unary->op, 1, x);
            }
            return NUM_VAL(-AS_NUM(x));
        }
        case TOKEN_BANG: {
            return BOOL_VAL(!isTruthy(x));
        }
        default:
            return NIL_VAL();
    }
}
