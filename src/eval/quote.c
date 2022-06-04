
#include "../eval.h"
#include "../quote.h"

#include <stdlib.h>

static val makeQuote(ast* ast) {
    switch(ast->type) {
        case AST_LITERAL: {
            astLiteral* literal = (astLiteral*)ast;
            if(literal->useValue)
                return literal->value;
            // TODO: make this work with tokens. needed mostly for compile time eval
        }
        case AST_UNARY: {
            astUnary* unary = (astUnary*)ast;
            return makeQuotedUnary(makeQuote(unary->val), unary->op.start, unary->op.line, unary->op.len, unary->op.type);
        }
        case AST_BINARY: {
            astBinary* binary = (astBinary*)ast;
            return makeQuotedBinary(
                makeQuote(binary->lhs),
                makeQuote(binary->rhs),
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
            return makeQuotedEval(makeQuote(eval->expr));
        }
    }
}

val evalQuote(engine* engine, astQuote* quote) {
    if(quote->first == NULL)
        return NIL_VAL();
    if(quote->first->next == NULL) {
        return makeQuote(quote->first);
    }
    // TODO: come back after i add arrays and fix this
}
