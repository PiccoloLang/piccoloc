
#include "../eval.h"
#include "../quote.h"

#include <stdlib.h>

static val makeQuote(engine* engine, ast* ast);

static val makeBlockQuote(engine* engine, ast* expr) {
    astBlock* block = (astBlock*)expr;
    int exprCnt = 0;
    ast* curr = block->first;
    while(curr != NULL) {
        exprCnt++;
        curr = curr->next;
    }
    val elems[exprCnt];
    curr = block->first;
    for(int i = 0; i < exprCnt; i++) {
        elems[i] = makeQuote(engine, curr);
        curr = curr->next;
    }
    return makeQuotedBlockFromArr(exprCnt, elems);
}

static val makeQuote(engine* engine, ast* ast) {
    switch(ast->type) {
        case AST_LITERAL: {
            astLiteral* literal = (astLiteral*)ast;
            if(literal->useValue)
                return literal->value;
            // TODO: make this work with tokens. needed mostly for compile time eval
        }
        case AST_BLOCK: {
            return makeBlockQuote(engine, ast);
        }
        case AST_QUOTE: {
            astQuote* quote = (astQuote*)ast;
            return OBJ_VAL(quote);
        }
        case AST_INQUOTE: {
            astInquote* inquote = (astInquote*)ast;
            return eval(engine, inquote->expr);
        }
        #include "quote_gen.c"
    }
}

val evalQuote(engine* engine, astQuote* quote) {
    if(quote->first == NULL)
        return NIL_VAL();
    if(quote->first->next == NULL) {
        return makeQuote(engine, quote->first);
    }
    // TODO: come back after i add arrays and fix this
    return NIL_VAL();
}
