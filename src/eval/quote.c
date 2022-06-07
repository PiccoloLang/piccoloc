
#include "../eval.h"
#include "../quote.h"

#include <stdlib.h>

static val makeQuote(engine* engine, ast* ast) {
    switch(ast->type) {
        case AST_LITERAL: {
            astLiteral* literal = (astLiteral*)ast;
            if(literal->useValue)
                return literal->value;
            // TODO: make this work with tokens. needed mostly for compile time eval
        }
        case AST_UNARY: {
            astUnary* unary = (astUnary*)ast;
            return makeQuotedUnary(makeQuote(engine, unary->val), unary->op.start, unary->op.line, unary->op.len, unary->op.type);
        }
        case AST_BINARY: {
            astBinary* binary = (astBinary*)ast;
            return makeQuotedBinary(
                makeQuote(engine, binary->lhs),
                makeQuote(engine, binary->rhs),
                binary->op.start,
                binary->op.line,
                binary->op.len, 
                binary->op.type
            );
        }
        case AST_QUOTE: {
            astQuote* quote = (astQuote*)ast;
            return OBJ_VAL(quote);
        }
        case AST_EVAL: {
            astEval* eval = (astEval*)ast;
            return makeQuotedEval(makeQuote(engine, eval->expr));
        }
        case AST_INQUOTE: {
            astInquote* inquote = (astInquote*)ast;
            return eval(engine, inquote->expr);
        }
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
