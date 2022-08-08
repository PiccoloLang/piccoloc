
#include "var_find.h"
#include "compiler.h"

void findVars(compiler* comp, ast* expr) {

    if(expr == NULL)
        return;

    switch(expr->type) {
        case AST_BLOCK:
        case AST_QUOTE: {
            break; // :^)
        }
        case AST_VARDECL: {
            astVarDecl* varDecl = (astVarDecl*)expr;
            variable var;
            var.name = varDecl->name;
            var.idx = comp->vars.cnt;
            var.declChrono = varDecl->ast.chrono;
            var.initialized = false;
            var.invalidated = false;
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
