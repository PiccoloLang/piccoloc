
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

typedef enum {
    PREC_NONE,
    PREC_LOGIC,
    PREC_EQUALITY,
    PREC_COMPARISON,
    PREC_ADD,
    PREC_MUL,
    PREC_CALL,
    PREC_ALL
} precedence;

typedef struct {
    ast* (*prefix)(parser* prsr);
    ast* (*infix)(parser* prsr, ast* left);
    precedence prec;
} parseRule;

parseRule parseRules[];



static void error(parser* prsr, const char* msg) {
    printf("Parsing Error: %s\n", msg);
    printf("Line %d| ", prsr->curr.line);

    const char* lineStart = prsr->curr.start;
    while(lineStart != prsr->scnr.src && *lineStart != '\n')
        lineStart--;
    if(*lineStart == '\n')
        lineStart++;

    const char* lineEnd = lineStart;
    while(*lineEnd != '\n' && *lineEnd != '\0')
        lineEnd++;

    printf("%.*s\n", (int)(lineEnd - lineStart), lineStart);
    
    int chars = 7;
    int line = prsr->curr.line;
    while(line > 0) {
        chars++;
        line /= 10;
    }
    chars += prsr->curr.start - lineStart;
    for(int i = 0; i < chars; i++)
        printf(" ");
    printf("^\n");

    prsr->hadError = true;
}

static void advance(parser* prsr) {
    prsr->curr = scanToken(&prsr->scnr);
    while(prsr->curr.type == TOKEN_ERR) {
        error(prsr, "Malformed token.");
        prsr->curr = scanToken(&prsr->scnr);
    }
}

static bool consume(parser* prsr, tokenType token) {
    while(prsr->curr.type == TOKEN_NL)
        advance(prsr);
    if(prsr->curr.type == token) {
        advance(prsr);
        return true;
    }
    return false;
}

static ast* allocAst(parser* prsr, astType type, size_t size) {
    ast* ast = malloc(size);
    ast->type = type;
    ast->next = NULL;
    ast->nextToFree = prsr->nodesToFree;
    prsr->nodesToFree = ast;
    return ast;
}

#define ALLOC_AST(prsr, type, typeEnum) (ast ## type*)(allocAst(prsr, AST_ ## typeEnum, sizeof(ast ## type)))


ast* parseExprList(parser* prsr, tokenType closingToken) {
    ast* first = NULL;
    ast* curr = NULL;
    while(prsr->curr.type == TOKEN_NL)
        advance(prsr);
    while(prsr->curr.type != closingToken && prsr->curr.type != TOKEN_EOF) {
        ast* ast = parse(prsr);
        if(curr == NULL) {
            curr = ast;
            first = ast;
        } else {
            curr->next = ast;
            curr = ast;
        }
        while(prsr->curr.type == TOKEN_NL)
            advance(prsr);
    }
    return first;
}

static ast* parseWithPrec(parser* prsr, precedence prec) {
    while(prsr->curr.type == TOKEN_NL)
        advance(prsr);

    if(parseRules[prsr->curr.type].prefix == NULL) {
        error(prsr, "Expected expression.");
        advance(prsr);
        return NULL;
    }

    ast* res = parseRules[prsr->curr.type].prefix(prsr);
    while(parseRules[prsr->curr.type].infix != NULL && parseRules[prsr->curr.type].prec > prec) {
        res = parseRules[prsr->curr.type].infix(prsr, res);
    }
    return res;
}

static ast* parseLiteral(parser* prsr) {
    astLiteral* res = ALLOC_AST(prsr, Literal, LITERAL);
    res->literal = prsr->curr;
    advance(prsr);
    return (ast*)res;
}

static ast* parseVar(parser* prsr) {
    astVar* res = ALLOC_AST(prsr, Var, VAR);
    res->name = prsr->curr;
    advance(prsr);
    return (ast*)res;
}

static ast* parseUnary(parser* prsr) {
    astUnary* res = ALLOC_AST(prsr, Unary, UNARY);
    res->op = prsr->curr;
    advance(prsr);
    res->val = parseWithPrec(prsr, PREC_ALL);
    return (ast*)res;
}

static ast* parseBinary(parser* prsr, ast* lhs) {
    astBinary* res = ALLOC_AST(prsr, Binary, BINARY);
    res->op = prsr->curr;
    precedence prec = parseRules[prsr->curr.type].prec;
    advance(prsr);
    res->lhs = lhs;
    res->rhs = parseWithPrec(prsr, prec);
    return (ast*)res;
}

static ast* parseParen(parser* prsr) {
    advance(prsr);
    ast* res = parse(prsr);
    if(!consume(prsr, TOKEN_RIGHT_PAREN)) {
        error(prsr, "Expected ).");
    }
    return res;
}

static ast* parseBlock(parser* prsr) {
    advance(prsr);
    astBlock* res = ALLOC_AST(prsr, Block, BLOCK);
    res->first = parseExprList(prsr, TOKEN_RIGHT_BRACE);
    if(!consume(prsr, TOKEN_RIGHT_BRACE)) {
        error(prsr, "Expected }.");
    }
    return (ast*)res;
}

static ast* parseCall(parser* prsr, ast* lhs) {
    astCall* call = ALLOC_AST(prsr, Call, CALL);
    call->fn = lhs;

    call->tkn = prsr->curr;
    advance(prsr);

    ast* currArg = NULL;
    while(prsr->curr.type == TOKEN_NL)
        advance(prsr);
    while(prsr->curr.type != TOKEN_RIGHT_PAREN) {
        ast* arg = parse(prsr);

        if(!consume(prsr, TOKEN_COMMA) && prsr->curr.type != TOKEN_RIGHT_PAREN) {
            error(prsr, "Expected comma.");
        }

        if(currArg == NULL) {
            call->args = (ast*)arg;
        } else {
            currArg->next = (ast*)arg;
        }
        currArg = arg;
    }
    
    if(!consume(prsr, TOKEN_RIGHT_PAREN)) {
        error(prsr, "Expected ).");
    }

    return (ast*)call;
}

static ast* parseQuote(parser* prsr) {
    advance(prsr);
    astQuote* res = ALLOC_AST(prsr, Quote, QUOTE);
    res->first = parseExprList(prsr, TOKEN_RIGHT_PAREN);
    if(!consume(prsr, TOKEN_RIGHT_PAREN)) {
        error(prsr, "Expected >.");
    }
    return (ast*)res;
}

static ast* parseEval(parser* prsr) {
    advance(prsr);
    astEval* res = ALLOC_AST(prsr, Eval, EVAL);
    res->expr = parseWithPrec(prsr, PREC_ALL);
    return (ast*)res;
}

static ast* parseInquote(parser* prsr) {
    token op = prsr->curr;
    advance(prsr);
    astInquote* res = ALLOC_AST(prsr, Inquote, INQUOTE);
    res->expr = parseWithPrec(prsr, PREC_ALL);
    res->op = op;
    return (ast*)res;
}

static ast* parseVarDecl(parser* prsr) {
    astVarDecl* res = ALLOC_AST(prsr, VarDecl, VARDECL);
    res->constant = prsr->curr.type == TOKEN_CONST;
    advance(prsr);

    if(prsr->curr.type != TOKEN_IDEN) {
        error(prsr, "Expected variable name.");
    } else {
        res->name = prsr->curr;
        advance(prsr);
    }

    if(prsr->curr.type != TOKEN_EQ) {
        error(prsr, "Expected =.");
    } else {
        advance(prsr);
    }

    res->expr = parseWithPrec(prsr, PREC_NONE);
    return (ast*)res;
}

static ast* parseFn(parser* prsr) {
    astFn* res = ALLOC_AST(prsr, Fn, FN);

    advance(prsr);

    ast* currArg = NULL;
    while(prsr->curr.type == TOKEN_NL)
        advance(prsr);
    while(prsr->curr.type == TOKEN_IDEN) {
        astVar* arg = ALLOC_AST(prsr, Var, VAR);
        arg->name = prsr->curr;

        advance(prsr);
        
        if(!consume(prsr, TOKEN_COMMA) && prsr->curr.type != TOKEN_ARROW) {
            error(prsr, "Expected comma.");
        }

        if(currArg == NULL) {
            res->args = (ast*)arg;
        } else {
            currArg->next = (ast*)arg;
        }
        currArg = (ast*)arg;
    }

    if(!consume(prsr, TOKEN_ARROW)) {
        error(prsr, "Expected ->.");
    }

    res->body = parseWithPrec(prsr, PREC_NONE);
    return (ast*)res;
}

ast* parse(parser* prsr) {
    return parseWithPrec(prsr, PREC_NONE);
}

parseRule parseRules[] = {
    [TOKEN_PLUS]        = {parseUnary,   parseBinary, PREC_ADD},
    [TOKEN_MINUS]       = {parseUnary,   parseBinary, PREC_ADD},
    [TOKEN_STAR]        = {parseEval,    parseBinary, PREC_MUL},
    [TOKEN_SLASH]       = {NULL,         parseBinary, PREC_MUL},
    [TOKEN_LEFT_PAREN]  = {parseParen,   parseCall,   PREC_CALL},
    [TOKEN_RIGHT_PAREN] = {NULL,         NULL,        PREC_NONE},
    [TOKEN_OPEN_QUOTE]  = {parseQuote,   NULL,        PREC_NONE},
    [TOKEN_LEFT_BRACE]  = {parseBlock,   NULL,        PREC_NONE},
    [TOKEN_RIGHT_BRACE] = {NULL,         NULL,        PREC_NONE},
    [TOKEN_EQ_EQ]       = {NULL,         parseBinary, PREC_EQUALITY},
    [TOKEN_GREATER]     = {NULL,         parseBinary, PREC_COMPARISON},
    [TOKEN_LESS]        = {NULL,         parseBinary, PREC_COMPARISON},
    [TOKEN_GREATER_EQ]  = {NULL,         parseBinary, PREC_COMPARISON},
    [TOKEN_LESS_EQ]     = {NULL,         parseBinary, PREC_COMPARISON},
    [TOKEN_ARROW]       = {NULL,         NULL,        PREC_NONE},
    [TOKEN_BANG]        = {parseUnary,   NULL,        PREC_NONE},
    [TOKEN_EQ]          = {NULL,         NULL,        PREC_NONE},
    [TOKEN_COMMA]       = {parseInquote, NULL,        PREC_NONE},
    [TOKEN_TRUE]        = {parseLiteral, NULL,        PREC_NONE},
    [TOKEN_FALSE]       = {parseLiteral, NULL,        PREC_NONE},
    [TOKEN_NIL]         = {parseLiteral, NULL,        PREC_NONE},
    [TOKEN_AND]         = {NULL,         parseBinary, PREC_LOGIC},
    [TOKEN_OR]          = {NULL,         parseBinary, PREC_LOGIC},
    [TOKEN_VAR]         = {parseVarDecl, NULL,        PREC_NONE},
    [TOKEN_CONST]       = {parseVarDecl, NULL,        PREC_NONE},
    [TOKEN_FN]          = {parseFn,      NULL,        PREC_NONE},
    [TOKEN_NUM]         = {parseLiteral, NULL,        PREC_NONE},
    [TOKEN_IDEN]        = {parseVar,     NULL,        PREC_NONE},
    [TOKEN_STRING]      = {parseLiteral, NULL,        PREC_NONE},
    [TOKEN_NL] = {0},
    [TOKEN_EOF] = {0}
};



void initParser(parser* prsr, const char* src) {
    initScanner(&prsr->scnr, src);
    advance(prsr);
    prsr->nodesToFree = NULL;
    prsr->hadError = false;
}

void freeParser(parser* prsr) {
    ast* curr = prsr->nodesToFree;
    while(curr != NULL) {
        ast* next = curr->nextToFree;
        free(curr);
        curr = next;
    }
    prsr->nodesToFree = NULL;
}

bool parserEOF(parser* prsr) {
    while(prsr->curr.type == TOKEN_NL)
        advance(prsr);
    return prsr->curr.type == TOKEN_EOF;
}
