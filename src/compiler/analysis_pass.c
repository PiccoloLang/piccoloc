
#include "analysis_pass.h"

#include <stdlib.h>

void analyze(ast* expr) {
    if(expr == NULL)
        return;
    expr->containsEval = false;
    switch(expr->type) {
        case AST_EVAL: {
            expr->containsEval = true;
            break;
        }
        case AST_BLOCK: {
            break;
        }
        #include "analysis_gen.c"
    }
    analyze(expr->next);
} 
