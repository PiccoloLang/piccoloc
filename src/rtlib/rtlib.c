
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "../value.h"
#include "../ast.h"
#include "obj.h"

void printVal(val value);

static void printAst(ast* expr) {
    switch(expr->type) {
        case AST_LITERAL: {
            astLiteral* literal = (astLiteral*)expr;
            printVal(literal->value);
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

void runtimeError(int lineNum, int offset, const char* line, const char* msg, int argc, ...) {
    printf("Runtime Error: ");
    va_list args;
    va_start(args, argc);
    for(const char* c = msg; *c != '\0'; c++) {
        if(*c == '@') {
            val v = va_arg(args, val);
            printf("%s", getTypename(v));
        } else {
            printf("%c", *c);
        }
    }
    va_end(args);
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

val makeString(const char* str) {
    objStr* strObj = ALLOC_OBJ(Str, STR);
    strObj->len = strlen(str);
    strObj->str = malloc(strObj->len + 1);
    memcpy(strObj->str, str, strObj->len);
    strObj->str[strObj->len] = '\0';
    return OBJ_VAL(strObj);
}

val concatStrings(objStr* s1, objStr* s2) {
    char buff[s1->len + s2->len + 1];
    memcpy(buff, s1->str, s1->len);
    memcpy(buff + s1->len, s2->str, s2->len + 1);
    return makeString(buff);
}

val add(val v1, val v2) {
    if(IS_OBJ(v1) && IS_OBJ(v2)) {
        obj* o1 = AS_OBJ(v1);
        obj* o2 = AS_OBJ(v2);
        if(o1->type == OBJ_STR && o2->type == OBJ_STR)
            return concatStrings((objStr*)o1, (objStr*)o2);
    }
    return NIL_VAL();
}