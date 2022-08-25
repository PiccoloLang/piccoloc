
#ifndef EVAL_H
#define EVAL_H

#include "value.h"
#include "ast.h"
#include "dynarr.h"

typedef struct rtVariable {
    const char* nameStart;
    int nameLen;
    struct rtVariable* prevVar; // TODO: linear lookup is slow, but I'm too lazy to add hashmaps

    bool usePtr;
    union {
        val* ptr;
        val val;
    } as;
} rtVariable;

struct engine;

typedef void (*rtErrCallbackType)(struct engine* engine, const char* msg, token tkn, int argc, ...);

typedef struct engine {
    const char* src;
    rtErrCallbackType runtimeError;
    rtVariable* currVar;
} engine;

void initEngine(engine* engine, rtErrCallbackType runtimeError);

val eval(engine* engine, ast* ast);
val evalLiteral(engine* engine, astLiteral* literal);
val evalVar(engine* engine, astVar* var);
val evalUnary(engine* engine, astUnary* unary);
val evalBinary(engine* engine, astBinary* binary);
val evalBlock(engine* engine, astBlock* block);
val evalCall(engine* engine, astCall* call);
val evalQuote(engine* engine, astQuote* quote);
val evalEval(engine* engine, astEval* eval);
val evalVarDecl(engine* engine, astVarDecl* varDecl);
val evalFn(engine* engine, astFn* fn);

bool isTruthy(val v);
val add(val v1, val v2);
val makeString(const char* str);

rtVariable* makeVariable(engine* engine);
rtVariable* findRtVar(engine* engine, token name);
void returnToVar(engine* engine, rtVariable* var);

#endif
