
#include "../token.h"
#include "../value.h"
#include "../ast.h"
#include "../obj.h"
#include "obj.h"

#include <stdarg.h>

#define ALLOC_AST(type) (ast ## type*)allocObj(sizeof(ast ## type), OBJ_QUOTE)

extern void print(val value);

static ast* valToAst(val value) {
    if(IS_OBJ(value)) {
        obj* o = AS_OBJ(value);
        if(o->type == OBJ_QUOTE) {
            return (ast*)o;
        }
    }
    astLiteral* literal = ALLOC_AST(Literal);
    literal->ast.type = AST_LITERAL;
    literal->useValue = true;
    literal->value = value;
    return (ast*)literal;
}

val makeQuotedUnary(val value, const char* start, int line, int len, int type) {
    astUnary* quoteObj = ALLOC_AST(Unary);
    quoteObj->ast.type = AST_UNARY;

    quoteObj->val = valToAst(value);

    quoteObj->op.start = start;
    quoteObj->op.line = line;
    quoteObj->op.len = len;
    quoteObj->op.type = type;

    return OBJ_VAL(quoteObj);
}

val makeQuotedBinary(val lhs, val rhs, const char* start, int line, int len, int type) {
    astBinary* quoteObj = ALLOC_AST(Binary);
    quoteObj->ast.type = AST_BINARY;

    quoteObj->lhs = valToAst(lhs);
    quoteObj->rhs = valToAst(rhs);

    quoteObj->op.start = start;
    quoteObj->op.line = line;
    quoteObj->op.len = len;
    quoteObj->op.type = type;

    return OBJ_VAL(quoteObj);
}

val makeQuotedQuote(int exprCnt, ...) {
    astQuote* quoteObj = ALLOC_AST(Quote);
    quoteObj->ast.type = AST_QUOTE;

    va_list args;
    va_start(args, exprCnt);

    if(exprCnt == 0) {
        quoteObj->first = NULL;
    } else {
        quoteObj->first = valToAst(va_arg(args, val));
        ast* curr = quoteObj->first;
        for(int i = 1; i < exprCnt; i++) {
            curr->next = valToAst(va_arg(args, val));
            curr = curr->next;
        }
    }

    va_end(args);

    return OBJ_VAL(quoteObj);
}

val makeQuotedEval(val expr) {
    astEval* eval = ALLOC_AST(Eval);
    eval->ast.type = AST_EVAL;
    
    eval->expr = valToAst(expr);

    return OBJ_VAL(eval);
}
