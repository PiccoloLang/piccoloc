
#ifndef BUILDERS_H
#define BUILDERS_H

#include <llvm-c/Core.h>
#include "../compiler.h"
#include "../../ast.h"
#include "../../value.h"
#include "../../token.h"

// TODO: clean up, make better names

LLVMValueRef buildInt(compiler* comp, int val);
LLVMValueRef buildBool(compiler* comp, bool val);
LLVMValueRef buildValue(compiler* comp, val val);

LLVMValueRef buildValToNum(compiler* comp, LLVMValueRef val);
LLVMValueRef buildNumToVal(compiler* comp, LLVMValueRef val);
LLVMValueRef buildBoolToVal(compiler* comp, LLVMValueRef val);
LLVMValueRef buildPtrToVal(compiler* comp, LLVMValueRef ptr);
LLVMValueRef buildValToPtr(compiler* comp, LLVMValueRef ptr);

LLVMValueRef buildIntEq(compiler* comp, LLVMValueRef a, LLVMValueRef b);

LLVMValueRef buildDeref(compiler* comp, LLVMValueRef ptr);
void buildStore(compiler* comp, LLVMValueRef ptr, LLVMValueRef val);

LLVMValueRef buildNumAdd(compiler* comp, LLVMValueRef a, LLVMValueRef b);
LLVMValueRef buildNumSub(compiler* comp, LLVMValueRef a, LLVMValueRef b);
LLVMValueRef buildNumMul(compiler* comp, LLVMValueRef a, LLVMValueRef b);
LLVMValueRef buildNumDiv(compiler* comp, LLVMValueRef a, LLVMValueRef b);
LLVMValueRef buildNumCmp(compiler* comp, LLVMValueRef a, LLVMValueRef b);
LLVMValueRef buildDouble(compiler* comp, double val);

LLVMValueRef buildPtrOffset(compiler* comp, LLVMValueRef ptr, LLVMValueRef offset);

LLVMValueRef buildString(compiler* comp, const char* s);

LLVMValueRef buildAnd(compiler* comp, LLVMValueRef a, LLVMValueRef b);
LLVMValueRef buildNot(compiler* comp, LLVMValueRef val);

LLVMBasicBlockRef makeBlock(compiler* comp);
void buildJump(compiler* comp, LLVMBasicBlockRef block);
typedef struct {
    LLVMBasicBlockRef trueCond;
    LLVMBasicBlockRef falseCond;
} buildChoiceRes;
buildChoiceRes buildChoice(compiler* comp, LLVMValueRef cond);

LLVMValueRef buildValAlloc(compiler* comp);
LLVMValueRef buildValArrAlloc(compiler* comp, LLVMValueRef cnt);
LLVMValueRef buildPtrAlloc(compiler* comp);
LLVMValueRef buildGetPtr(compiler* comp, LLVMValueRef ptr);
LLVMValueRef buildGetPtrPtr(compiler* comp, LLVMValueRef ptr);
LLVMValueRef buildSetPtr(compiler* comp, LLVMValueRef ptr, LLVMValueRef val);

void compileToBlock(compiler* comp, LLVMBasicBlockRef block);

LLVMValueRef buildIsNilCheck(compiler* comp, LLVMValueRef val);
LLVMValueRef buildIsUninitCheck(compiler* comp, LLVMValueRef val);
LLVMValueRef buildIsNumCheck(compiler* comp, LLVMValueRef val);
LLVMValueRef buildIsBoolCheck(compiler* comp, LLVMValueRef val);
LLVMValueRef buildIsObjCheck(compiler* comp, LLVMValueRef val);
LLVMValueRef buildTruthy(compiler* comp, LLVMValueRef val);

LLVMValueRef buildCall(compiler* comp, LLVMTypeRef type, LLVMValueRef func, int argc, ...);
LLVMValueRef buildCallFromArgArray(compiler* comp, LLVMTypeRef type, LLVMValueRef func, int argc, LLVMValueRef* args);
void buildRuntimeError(compiler* comp, token tkn, const char* msg, int argc, ...);

LLVMValueRef buildStrOffset(compiler* comp, LLVMValueRef str, LLVMValueRef offset);

LLVMValueRef buildLiteral(compiler* comp, astLiteral* ast);
LLVMValueRef buildVar(compiler* comp, astVar* ast);
LLVMValueRef buildUnary(compiler* comp, astUnary* ast);
LLVMValueRef buildBinary(compiler* comp, astBinary* ast);
LLVMValueRef buildBlock(compiler* comp, astBlock* ast);
LLVMValueRef buildCallExpr(compiler* comp, astCall* ast);
LLVMValueRef buildQuote(compiler* comp, astQuote* ast);
LLVMValueRef buildEval(compiler* comp, astEval* ast);
LLVMValueRef buildVarDecl(compiler* comp, astVarDecl* ast);
LLVMValueRef buildFn(compiler* comp, astFn* ast);

#endif
