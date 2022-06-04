
#include "../eval.h"

val evalBinary(engine* engine, astBinary* binary) {
    val lhs = eval(engine, binary->lhs);

    if(binary->op.type == TOKEN_AND) {
        if(isTruthy(lhs))
            return eval(engine, binary->rhs);
        return lhs;
    }
    if(binary->op.type == TOKEN_OR) {
        if(isTruthy(lhs))
            return lhs;
        return eval(engine, binary->rhs);
    }

    val rhs = eval(engine, binary->rhs);

    #define NUM_OP(token, result, msg) \
        case TOKEN_ ## token: { \
            if(IS_NUM(lhs) && IS_NUM(rhs)) \
                return result; \
            engine->runtimeError(engine, msg, binary->op, 2, lhs, rhs); \
        }    
    
    switch(binary->op.type) {
        case TOKEN_PLUS: {
            if(IS_NUM(lhs) && IS_NUM(rhs))
                return NUM_VAL(AS_NUM(lhs) + AS_NUM(rhs));
            val res = add(lhs, rhs);
            if(res != NIL_VAL())
                return res;
            engine->runtimeError(engine, "Cannot add @ and @.", binary->op, 2, lhs, rhs);
        }
        case TOKEN_MINUS: {
            if(IS_NUM(lhs) && IS_NUM(rhs))
                return NUM_VAL(AS_NUM(lhs) - AS_NUM(rhs));
            engine->runtimeError(engine, "Cannot subtract @ from @.", binary->op, 2, rhs, lhs);
        }
        NUM_OP(STAR, NUM_VAL(AS_NUM(lhs) * AS_NUM(rhs)), "Cannot multiply @ and @.")
        NUM_OP(SLASH, NUM_VAL(AS_NUM(lhs) / AS_NUM(rhs)), "Cannot divide @ by @.")
        NUM_OP(GREATER, BOOL_VAL(AS_NUM(lhs) > AS_NUM(rhs)), "Cannot compare @ and @.")
        NUM_OP(LESS, BOOL_VAL(AS_NUM(lhs) < AS_NUM(rhs)), "Cannot compare @ and @.")
        NUM_OP(GREATER_EQ, BOOL_VAL(AS_NUM(lhs) >= AS_NUM(rhs)), "Cannot compare @ and @.")
        NUM_OP(LESS_EQ, BOOL_VAL(AS_NUM(lhs) <= AS_NUM(rhs)), "Cannot compare @ and @.")
        default:
            return NIL_VAL();
    }
}