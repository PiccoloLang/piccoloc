
#include "eval.h"
#include <string.h>

void initEngine(engine* engine, rtErrCallbackType runtimeError) {
    engine->runtimeError = runtimeError;
    engine->currVar = NULL;
}

val eval(engine* engine, ast* ast) {
    switch(ast->type) {
        case AST_LITERAL:
            return evalLiteral(engine, (astLiteral*)ast);
        case AST_VAR:
            return evalVar(engine, (astVar*)ast);
        case AST_UNARY:
            return evalUnary(engine, (astUnary*)ast);
        case AST_BINARY:
            return evalBinary(engine, (astBinary*)ast);
        case AST_BLOCK:
            return evalBlock(engine, (astBlock*)ast);
        case AST_CALL: 
            return evalCall(engine, (astCall*)ast);
        case AST_QUOTE:
            return evalQuote(engine, (astQuote*)ast);
        case AST_EVAL:
            return evalEval(engine, (astEval*)ast);
        case AST_INQUOTE: {
            astInquote* inquote = (astInquote*)ast;
            engine->runtimeError(engine, "Cannot inquote outside quote.", inquote->op, 0);
            return NIL_VAL();
        }
        case AST_VARDECL:
            return evalVarDecl(engine, (astVarDecl*)ast);
        case AST_FN: 
            return evalFn(engine, (astFn*)ast);
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


rtVariable* makeVariable(engine* engine) {
    rtVariable* var = malloc(sizeof(rtVariable)); // TODO: PLEASE ADD GC AAAAAAAA
    var->prevVar = engine->currVar;
    engine->currVar = var;
    return var;
}

rtVariable* findRtVar(engine* engine, token name) {
    rtVariable* curr = engine->currVar;
    while(curr != NULL) {
        if(curr->nameLen == name.len && strncmp(curr->nameStart, name.start, curr->nameLen) == 0)
            break;
        curr = curr->prevVar;
    }
    return curr;
}

void returnToVar(engine* engine, rtVariable* var) {
    engine->currVar = var;
}
