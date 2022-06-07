
#include "builders.h"

LLVMValueRef buildEval(compiler* comp, astEval* ast) {
    LLVMValueRef expr = compile(comp, ast->expr);
    return buildCall(comp, comp->rtlib->evaluateQuote.type, comp->rtlib->evaluateQuote.func, 1, expr);
}
