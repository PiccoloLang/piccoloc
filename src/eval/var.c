
#include "../eval.h"

#include <stdlib.h>

val evalVar(engine* engine, astVar* var) {
    
    rtVariable* rtVar = findRtVar(engine, var->name);
    if(rtVar == NULL) {
        engine->runtimeError(engine, "Variable does not exist.", var->name, 0);
        return NIL_VAL();
    }

    return rtVar->usePtr ? *rtVar->as.ptr : rtVar->as.val;

}
