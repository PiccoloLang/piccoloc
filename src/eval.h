
#ifndef EVAL_H
#define EVAL_H

#include "value.h"
#include "ast.h"
#include "dynarr.h"

typedef struct {
    const char* nameStart;
    int nameLen;
    val* ptr;
} rtVariable;

struct engine;

typedef void (*rtErrCallbackType)(struct engine* engine, const char* msg, token tkn, int argc, ...);

typedef struct engine {
    const char* src;
    rtErrCallbackType runtimeError;
    dynarr(rtVariable) vars; // Linear variable lookup is slow. Too bad!
} engine;

void initEngine(engine* engine, rtErrCallbackType runtimeError);

val eval(engine* engine, ast* ast);
val evalLiteral(engine* engine, astLiteral* literal);
val evalUnary(engine* engine, astUnary* unary);
val evalBinary(engine* engine, astBinary* binary);
val evalBlock(engine* engine, astBlock* block);
val evalQuote(engine* engine, astQuote* quote);
val evalEval(engine* engine, astEval* eval);

bool isTruthy(val v);
val add(val v1, val v2);
val makeString(const char* str);

#endif
