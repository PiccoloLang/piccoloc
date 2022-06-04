
#include "eval.h"

val eval(engine* engine, ast* ast) {
    switch(ast->type) {
        case AST_LITERAL:
            return evalLiteral(engine, (astLiteral*)ast);
        case AST_UNARY:
            return evalUnary(engine, (astUnary*)ast);
        case AST_BINARY:
            return evalBinary(engine, (astBinary*)ast);
        case AST_QUOTE:
            return evalQuote(engine, (astQuote*)ast);
        case AST_EVAL:
            return evalEval(engine, (astEval*)ast);
    }
}

bool isTruthy(val v) {
    return v != NIL_VAL() || v != BOOL_VAL(false);
}

static val concatStrings(objStr* s1, objStr* s2) {
    char buff[s1->len + s2->len + 1];
    memcpy(buff, s1->str, s1->len);
    memcpy(buff + s1->len, s2->str, s2->len + 1);
    return makeString(buff);
}

val add(val v1, val v2) {
    if(IS_OBJ(v1) && IS_OBJ(v2)) {
        obj* o1 = AS_OBJ(v1);
        obj* o2 = AS_OBJ(v2);
        if(o1->type == OBJ_STR && o2->type == OBJ_STR) {
            return concatStrings((objStr*)o1, (objStr*)o2);
        }
    }
    return NIL_VAL();
}