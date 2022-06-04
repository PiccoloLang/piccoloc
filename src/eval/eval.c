
#include "../eval.h"
#include "../ast.h"

val evalEval(engine* engine, astEval* evalExpr) {
    return eval(engine, evalExpr->expr);
}