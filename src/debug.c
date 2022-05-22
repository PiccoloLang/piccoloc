
#include "debug.h"

#include <stdio.h>

static void dumpAstWithOffset(ast* ast, int off) {
    for(int i = 0; i < off; i++) {
        printf("\t");
    }

    if(ast == NULL) {
        printf("null ast\n");
        return;
    }

    switch(ast->type) {
        case AST_LITERAL: {
            astLiteral* literal = (astLiteral*)ast;
            printf("literal '%.*s'\n", literal->literal.len, literal->literal.start);
            break;
        }
        case AST_UNARY: {
            astUnary* unary = (astUnary*)ast;
            printf("unary '%.*s'\n", unary->op.len, unary->op.start);
            dumpAstWithOffset(unary->val, off + 1);
            break;
        }
        case AST_BINARY: {
            astBinary* binary = (astBinary*)ast;
            printf("binary '%.*s'\n", binary->op.len, binary->op.start);
            dumpAstWithOffset(binary->lhs, off + 1);
            dumpAstWithOffset(binary->rhs, off + 1);
            break;
        }
    }
}

void dumpAst(ast* ast) {
    dumpAstWithOffset(ast, 0);
}