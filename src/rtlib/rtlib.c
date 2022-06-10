
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "../value.h"
#include "../ast.h"
#include "../obj.h"

#include "rtlib.h"

engine rtEngine;

void printVal(val value);

static void printAst(ast* expr) {
    switch(expr->type) {
        case AST_LITERAL: {
            astLiteral* literal = (astLiteral*)expr;
            printVal(literal->value);
            break;
        }
        case AST_VAR: {
            astVar* var = (astVar*)expr;
            printf("%.*s", var->name.len, var->name.start);
            break;
        }
        case AST_UNARY: {
            astUnary* unary = (astUnary*)expr;
            printf("%.*s", unary->op.len, unary->op.start);
            printAst(unary->val);
            break;
        }
        case AST_BINARY: {
            astBinary* binary = (astBinary*)expr;
            printAst(binary->lhs);
            printf(" %.*s ", binary->op.len, binary->op.start);
            printAst(binary->rhs);
            break;
        }
        case AST_BLOCK: {
            astBlock* block = (astBlock*)expr;
            printf("{");
            ast* curr = block->first;
            while(curr != NULL) {
                printAst(curr);
                if(curr->next != NULL)
                    printf("; ");
                curr = curr->next;
            }
            printf("}");
            break;
        }
        case AST_QUOTE: {
            astQuote* quote = (astQuote*)expr;
            printf("`(");
            ast* curr = quote->first;
            while(curr != NULL) {
                printAst(curr);
                if(curr->next != NULL)
                    printf("; ");
                curr = curr->next;
            }
            printf(")");
            break;
        }
        case AST_EVAL: {
            astEval* eval = (astEval*)expr;
            printf("*");
            printAst(eval->expr);
            break;
        }
        case AST_INQUOTE: {
            astInquote* inquote = (astInquote*)expr;
            printf(",");
            printAst(inquote->expr);
            break;
        }
        case AST_VARDECL: {
            astVarDecl* varDecl = (astVarDecl*)expr;
            printf("var %.*s = ", varDecl->name.len, varDecl->name.start); // TODO: add const here
            printAst(varDecl->expr);
            break;
        }
    }
}

static void printObj(obj* obj) {
    if(obj->type == OBJ_STR) {
        objStr* str = (objStr*)obj;
        printf("%s", str->str);
    } else if(obj->type == OBJ_QUOTE) {
        printf("`(");
        printAst((ast*)obj);
        printf(")");
    }
}

void printVal(val value) {
    if(IS_NUM(value))
        printf("%g", AS_NUM(value));
    else if(IS_BOOL(value))
        printf("%s", AS_BOOL(value) ? "true" : "false");
    else if(IS_NIL(value))
        printf("nil");
    else if(IS_OBJ(value))
        printObj(AS_OBJ(value));
}

void print(val value) {
    printVal(value);
    printf("\n");
}

static void runtimeErrorUtil(int lineNum, int offset, const char* line, const char* msg, va_list args) {
    printf("Runtime Error: ");
    for(const char* c = msg; *c != '\0'; c++) {
        if(*c == '@') {
            val v = va_arg(args, val);
            printf("%s", getTypename(v));
        } else {
            printf("%c", *c);
        }
    }
    printf("\n");
    printf("Line %d| %s\n", lineNum, line);

    int chars = 7;
    while(lineNum > 0) {
        lineNum /= 10;
        chars++;
    }
    for(int i = 0; i < chars + offset; i++)
        printf(" ");
    printf("^\n");
    
    exit(-1);
}

void runtimeError(int lineNum, int offset, const char* line, const char* msg, int argc, ...) {
    va_list args;
    va_start(args, argc);
    runtimeErrorUtil(lineNum, offset, line, msg, args);
}

void engineRuntimeErr(engine* engine, const char* msg, token tkn, int argc, ...) {
    va_list args;
    va_start(args, argc);
    const char* line = tkn.start;
    while(*line != '\n' && line != engine->src)
        line--;
    if(*line == '\n')
        line++;
    runtimeErrorUtil(tkn.line, tkn.start - engine->src - 1, line, msg, args);
}

void initRTLib(const char* src) {
    rtEngine.src = src;
    rtEngine.runtimeError = engineRuntimeErr;
}

val makeString(const char* str) {
    objStr* strObj = ALLOC_OBJ(Str, STR);
    strObj->len = strlen(str);
    strObj->str = malloc(strObj->len + 1);
    memcpy(strObj->str, str, strObj->len);
    strObj->str[strObj->len] = '\0';
    return OBJ_VAL(strObj);
}
