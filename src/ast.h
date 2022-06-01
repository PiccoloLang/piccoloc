
#ifndef AST_H
#define AST_H

#include "token.h"
#include "value.h"
#include "obj.h"
#include <stdbool.h>

typedef enum {
    AST_LITERAL,
    AST_UNARY,
    AST_BINARY,
    AST_QUOTE,
    AST_EVAL
} astType;

typedef struct ast {
    obj obj; // header used for quoting at runtime
    astType type;
    struct ast* next;
    struct ast* nextToFree; // used to free ast nodes after compilation
} ast;

typedef struct {
    ast ast;
    token literal;

    // used by the runtime when using literals inside of quotes
    val value;
    bool useValue;
} astLiteral;

typedef struct {
    ast ast;
    ast* val;
    token op;
} astUnary;

typedef struct {
    ast ast;
    token op;
    ast* lhs;
    ast* rhs;
} astBinary;

typedef struct {
    ast ast;
    ast* first;
} astQuote;

typedef struct {
    ast ast;
    ast* expr;
} astEval;

#endif