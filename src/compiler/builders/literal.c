
#include "builders.h"

#include <llvm-c/Core.h>

#include "../compiler.h"
#include "../../ast.h"
#include "../../value.h"

#include <stdlib.h>
#include <string.h>

LLVMValueRef buildLiteral(compiler* comp, astLiteral* literal) {
    switch(literal->literal.type) {
        case TOKEN_NUM: {
            double val = strtod(literal->literal.start, NULL);
            return buildValue(comp, NUM_VAL(val));
        }
        case TOKEN_TRUE:
            return buildValue(comp, BOOL_VAL(true));
        case TOKEN_FALSE:
            return buildValue(comp, BOOL_VAL(false));
        case TOKEN_NIL:
            return buildValue(comp, NIL_VAL());
        case TOKEN_STRING: {
            char str[literal->literal.len - 1];
            memcpy(str, literal->literal.start + 1, literal->literal.len - 2);
            str[literal->literal.len] = '\0';
            return buildCall(comp, comp->rtlib->makeString.type, comp->rtlib->makeString.func, 1, buildString(comp, str));
        }
        default:
            return NULL;
    }
}
