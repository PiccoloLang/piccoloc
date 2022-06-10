
#include "token.h"
#include "value.h"
#include "ast.h"
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

val makeQuotedBlockFromArr(int exprCnt, val* elems) {

    astBlock* quoteObj = ALLOC_AST(Block);
    quoteObj->ast.type = AST_BLOCK;

    if(exprCnt == 0) {
        quoteObj->first = NULL;
    } else {
        quoteObj->first = valToAst(elems[0]);
        ast* curr = quoteObj->first;
        for(int i = 1; i < exprCnt; i++) {
            curr->next = valToAst(elems[i]);
            curr = curr->next;
        }
    }

    return OBJ_VAL(quoteObj);
    
}

val makeQuotedBlock(int exprCnt, ...) {
    va_list args;
    va_start(args, exprCnt);

    val elems[exprCnt];
    for(int i = 0; i < exprCnt; i++)
        elems[i] = va_arg(args, val);
    
    va_end(args);

    return makeQuotedBlockFromArr(exprCnt, elems);
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

#include "quote_gen.c"
