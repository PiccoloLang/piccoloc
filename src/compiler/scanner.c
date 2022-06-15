
#include "scanner.h"

#include <stdbool.h>
#include <string.h>

void initScanner(scanner* scnr, const char* src) {
    scnr->src = src;
    scnr->start = src;
    scnr->curr = src;
    scnr->line = 1;
}

static void advance(scanner* scnr) {
    if(*scnr->curr == '\n')
        scnr->line++; 
    scnr->curr++;
}

static void skipWhitespace(scanner* scnr) {
    while(*scnr->curr == ' ' || *scnr->curr == '\t' || *scnr->curr == '\r')
        advance(scnr);
    scnr->start = scnr->curr;
}

static token makeToken(scanner* scnr, tokenType type) {
    token res;
    res.type = type;
    res.start = scnr->start;
    res.len = scnr->curr - scnr->start;
    res.line = scnr->line;

    scnr->start = scnr->curr;

    return res;
}

static bool numeric(char c) {
    return c >= '0' && c <= '9';
}

static bool alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '@' || c == '$';
}

static bool alphanum(char c) {
    return alpha(c) || numeric(c);
}

token scanToken(scanner* scnr) {
    skipWhitespace(scnr);
    char startChar = *scnr->start;
    
    if(startChar == '\0')
        return makeToken(scnr, TOKEN_EOF);
    if(startChar == '\n') {
        advance(scnr);
        return makeToken(scnr, TOKEN_NL);
    }

    #define SINGLE_CHAR(c, token) \
        if(startChar == c) { \
            advance(scnr); \
            return makeToken(scnr, TOKEN_ ## token); \
        }
    
    #define DOUBLE_CHAR(c1, c2, token1, token2) \
        if(startChar == c1) { \
            advance(scnr); \
            if(*scnr->curr == c2) { \
                advance(scnr); \
                return makeToken(scnr, TOKEN_ ## token2); \
            } \
            return makeToken(scnr, TOKEN_ ## token1); \
        }
    
    #define TWO_LONG_CHAR(c1, c2, token) \
        DOUBLE_CHAR(c1, c2, ERR, token)

    SINGLE_CHAR('+', PLUS)
    SINGLE_CHAR('-', MINUS)
    SINGLE_CHAR('*', STAR)
    SINGLE_CHAR('/', SLASH)
    SINGLE_CHAR('(', LEFT_PAREN)
    SINGLE_CHAR(')', RIGHT_PAREN)
    TWO_LONG_CHAR('`', '(', OPEN_QUOTE)
    SINGLE_CHAR('{', LEFT_BRACE)
    SINGLE_CHAR('}', RIGHT_BRACE)
    DOUBLE_CHAR('>', '=', GREATER, GREATER_EQ)
    DOUBLE_CHAR('<', '=', LESS, LESS_EQ)
    SINGLE_CHAR('!', BANG)
    SINGLE_CHAR('=', EQ)
    SINGLE_CHAR(',', COMMA)

    if(startChar == '\'') {
        advance(scnr);
        while(*scnr->curr != '\'')
            advance(scnr);
        advance(scnr);
        return makeToken(scnr, TOKEN_STRING);
    }
    if(numeric(startChar)) {
        while(numeric(*scnr->curr))
            advance(scnr);
        if(*scnr->curr == '.') {
            advance(scnr);
            while(numeric(*scnr->curr))
                advance(scnr);
        }
        return makeToken(scnr, TOKEN_NUM);
    }
    if(alpha(startChar)) {
        while(alphanum(*scnr->curr))
            advance(scnr);
        
        #define KEYWORD(keyword, enumWord) \
            if(scnr->curr - scnr->start == strlen(#keyword) && memcmp(scnr->start, #keyword, strlen(#keyword)) == 0) \
                return makeToken(scnr, TOKEN_ ## enumWord);

        KEYWORD(true, TRUE)
        KEYWORD(false, FALSE)
        KEYWORD(nil, NIL)
        KEYWORD(and, AND)
        KEYWORD(or, OR)
        KEYWORD(var, VAR)
        KEYWORD(const, CONST)

        return makeToken(scnr, TOKEN_IDEN);
    }
    
    advance(scnr);
    return makeToken(scnr, TOKEN_ERR);
}
