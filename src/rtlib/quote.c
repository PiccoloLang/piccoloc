
#include "../token.h"
#include "../value.h"
#include "../ast.h"
#include "../obj.h"
#include "obj.h"

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
    quoteObj->op.start = start;
    quoteObj->op.line = line;
    quoteObj->op.len = len;
    quoteObj->op.type = type;
    quoteObj->val = valToAst(value);
    return OBJ_VAL(quoteObj);
}