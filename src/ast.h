
#ifndef AST_H
#define AST_H

#include "token.h"

typedef enum {
    AST_LITERAL,
    AST_UNARY,
    AST_BINARY
} astType;

typedef struct ast {
    astType type;
    struct ast* nextToFree; // used to free ast nodes after compilation
} ast;

typedef struct {
    ast ast;
    token literal;
} astLiteral;

typedef struct {
    ast ast;
    token op;
    ast* val;
} astUnary;

typedef struct {
    ast ast;
    token op;
    ast* lhs;
    ast* rhs;
} astBinary;

#endif