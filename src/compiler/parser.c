
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

typedef enum {
    PREC_NONE,
    PREC_LOGIC,
    PREC_EQUALITY,
    PREC_COMPARISON,
    PREC_ADD,
    PREC_MUL,
    PREC_ALL
} precedence;

typedef struct {
    ast* (*prefix)(parser* prsr);
    ast* (*infix)(parser* prsr, ast* left);
    precedence prec;
} parseRule;

parseRule parseRules[];



static void error(parser* prsr, const char* msg) {
    printf("error lol. '%.*s'.\n", prsr->curr.len, prsr->curr.start);
    prsr->hadError = true;
}

static void advance(parser* prsr) {
    prsr->curr = scanToken(&prsr->scnr);
    while(prsr->curr.type == TOKEN_ERR) {
        error(prsr, "Malformed token.");
        prsr->curr = scanToken(&prsr->scnr);
    }
}

static ast* allocAst(parser* prsr, astType type, size_t size) {
    ast* ast = malloc(size);
    ast->type = type;
    ast->nextToFree = prsr->nodesToFree;
    prsr->nodesToFree = ast;
    return ast;
}

#define ALLOC_AST(prsr, type, typeEnum) (ast ## type*)(allocAst(prsr, AST_ ## typeEnum, sizeof(ast ## type)))



static ast* parseWithPrec(parser* prsr, precedence prec) {
    while(prsr->curr.type == TOKEN_NL)
        advance(prsr);

    if(parseRules[prsr->curr.type].prefix == NULL) {
        error(prsr, "Expected expression.");
        advance(prsr);
        return NULL;
    }

    ast* res = parseRules[prsr->curr.type].prefix(prsr);
    while(parseRules[prsr->curr.type].infix != NULL && parseRules[prsr->curr.type].prec > prec) {
        res = parseRules[prsr->curr.type].infix(prsr, res);
    }
    return res;
}

static ast* parseLiteral(parser* prsr) {
    astLiteral* res = ALLOC_AST(prsr, Literal, LITERAL);
    res->literal = prsr->curr;
    advance(prsr);
    return (ast*)res;
}

static ast* parseUnary(parser* prsr) {
    astUnary* res = ALLOC_AST(prsr, Unary, UNARY);
    res->op = prsr->curr;
    advance(prsr);
    res->val = parseWithPrec(prsr, PREC_ALL);
    return (ast*)res;
}

static ast* parseBinary(parser* prsr, ast* lhs) {
    astBinary* res = ALLOC_AST(prsr, Binary, BINARY);
    res->op = prsr->curr;
    precedence prec = parseRules[prsr->curr.type].prec;
    advance(prsr);
    res->lhs = lhs;
    res->rhs = parseWithPrec(prsr, prec);
    return (ast*)res;
}

static ast* parseParen(parser* prsr) {
    advance(prsr);
    ast* res = parse(prsr);
    while(prsr->curr.type == TOKEN_NL)
        advance(prsr);
    if(prsr->curr.type != TOKEN_RIGHT_PAREN) {
        error(prsr, "Expected ).");
    } else {
        advance(prsr);
    }
    return res;
}

ast* parse(parser* prsr) {
    return parseWithPrec(prsr, PREC_NONE);
}

parseRule parseRules[] = {
    [TOKEN_PLUS]        = {parseUnary,   parseBinary, PREC_ADD},
    [TOKEN_MINUS]       = {parseUnary,   parseBinary, PREC_ADD},
    [TOKEN_STAR]        = {NULL,         parseBinary, PREC_MUL},
    [TOKEN_SLASH]       = {NULL,         parseBinary, PREC_MUL},
    [TOKEN_LEFT_PAREN]  = {parseParen,   NULL,        PREC_NONE},
    [TOKEN_RIGHT_PAREN] = {NULL,         NULL,        PREC_NONE},
    [TOKEN_GREATER]     = {NULL,         parseBinary, PREC_COMPARISON},
    [TOKEN_LESS]        = {NULL,         parseBinary, PREC_COMPARISON},
    [TOKEN_GREATER_EQ]  = {NULL,         parseBinary, PREC_COMPARISON},
    [TOKEN_LESS_EQ]     = {NULL,         parseBinary, PREC_COMPARISON},
    [TOKEN_BANG]        = {parseUnary,   NULL,        PREC_NONE},
    [TOKEN_TRUE]        = {parseLiteral, NULL,        PREC_NONE},
    [TOKEN_FALSE]       = {parseLiteral, NULL,        PREC_NONE},
    [TOKEN_NIL]         = {parseLiteral, NULL,        PREC_NONE},
    [TOKEN_AND]         = {NULL,         parseBinary, PREC_LOGIC},
    [TOKEN_OR]          = {NULL,         parseBinary, PREC_LOGIC},
    [TOKEN_NUM]         = {parseLiteral, NULL,        PREC_NONE},
    [TOKEN_IDEN]        = {NULL,         NULL,        PREC_NONE},
    [TOKEN_STRING]      = {parseLiteral, NULL,        PREC_NONE}
};



void initParser(parser* prsr, const char* src) {
    initScanner(&prsr->scnr, src);
    advance(prsr);
    prsr->nodesToFree = NULL;
    prsr->hadError = false;
}

void freeParser(parser* prsr) {
    ast* curr = prsr->nodesToFree;
    while(curr != NULL) {
        ast* next = curr->nextToFree;
        free(curr);
        curr = next;
    }
    prsr->nodesToFree = NULL;
}

bool parserEOF(parser* prsr) {
    while(prsr->curr.type == TOKEN_NL)
        advance(prsr);
    return prsr->curr.type == TOKEN_EOF;
}