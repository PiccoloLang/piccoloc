
#include "var_find.h"

void findVars(compiler* comp, ast* expr) {
    switch(expr->type) {
        case AST_BLOCK: {
            break; // :^)
        }
        case AST_VARDECL: {
            astVarDecl* varDecl = (astVarDecl*)expr;
            variable var;
            var.name = varDecl->name;
            var.idx = comp->vars.cnt;
            varDecl->idx = var.idx;
            pushDynarr(&comp->vars, var);
            findVars(comp, varDecl->expr);
            break;
        }
        #include "var_find_gen.c"
    }
    if(expr->next != NULL)
        findVars(comp, expr->next);
}
