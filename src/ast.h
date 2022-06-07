
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
	AST_EVAL,
} astType;

typedef struct ast {
	obj obj; // header used for runtime quoting
	astType type;
	struct ast* next;
	struct ast* nextToFree; // used to free nodes at compiletime
} ast;

typedef struct {
	ast ast;
	token literal;
	val value; // used by runtime quoting
	bool useValue;
} astLiteral;

typedef struct {
	ast ast;
	ast* val;
	token op;
} astUnary;

typedef struct {
	ast ast;
	ast* lhs;
	ast* rhs;
	token op;
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

