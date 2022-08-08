
#include "../eval.h"

val evalVarDecl(engine* engine, astVarDecl* varDecl) {
    rtVariable* var = makeVariable(engine);
    var->nameStart = varDecl->name.start;
    var->nameLen = varDecl->name.len;

    var->usePtr = false;
    var->as.val = eval(engine, varDecl->expr);
    
    return var->as.val;
}
