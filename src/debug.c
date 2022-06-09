
#include "debug.h"
#include "ast.h"

#include <stdio.h>

static void dumpAstWithOffset(ast* expr, int off) {
    for(int i = 0; i < off; i++) {
        printf("\t");
    }

    if(expr == NULL) {
        printf("null ast\n");
        return;
    }

    switch(expr->type) {
        case AST_LITERAL: {
            astLiteral* literal = (astLiteral*)expr;
            printf("literal '%.*s'\n", literal->literal.len, literal->literal.start);
            break;
        }
        case AST_VAR: {
            astVar* var = (astVar*)expr;
            printf("var '%.*s'\n", var->name.len, var->name.start);
            break;
        }
        case AST_UNARY: {
            astUnary* unary = (astUnary*)expr;
            printf("unary '%.*s'\n", unary->op.len, unary->op.start);
            dumpAstWithOffset(unary->val, off + 1);
            break;
        }
        case AST_BINARY: {
            astBinary* binary = (astBinary*)expr;
            printf("binary '%.*s'\n", binary->op.len, binary->op.start);
            dumpAstWithOffset(binary->lhs, off + 1);
            dumpAstWithOffset(binary->rhs, off + 1);
            break;
        }
        case AST_QUOTE: {
            astQuote* quote = (astQuote*)expr;
            printf("quote\n");
            ast* curr = quote->first;
            while(curr != NULL) {
                dumpAstWithOffset(curr, off + 1);
                curr = curr->next;
            }
            break;
        }
        case AST_EVAL: {
            astEval* eval = (astEval*)expr;
            printf("eval\n");
            dumpAstWithOffset(eval->expr, off + 1);
            break;
        }
        case AST_INQUOTE: {
            astInquote* inquote = (astInquote*)expr;
            printf("inquote\n");
            dumpAstWithOffset(inquote->expr, off + 1);
            break;
        }
        case AST_VARDECL: {
            astVarDecl* varDecl = (astVarDecl*)expr;
            printf("vardecl %.*s\n", varDecl->name.len, varDecl->name.start);
            break;
        }
    }
}

void dumpAst(ast* ast) {
    dumpAstWithOffset(ast, 0);
}
