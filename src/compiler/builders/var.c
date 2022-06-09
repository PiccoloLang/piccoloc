
#include "builders.h"

LLVMValueRef buildVar(compiler* comp, astVar* var) {
    int idx = findVar(comp, var->name);
    if(idx == -1) {
        // TODO: find new variables at runtime
        return NULL;
    } else {
        return buildGetPtr(comp, comp->vars.vals[idx].alloc);
    }
}
