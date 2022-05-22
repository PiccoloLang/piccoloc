
#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH,

    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,

    TOKEN_GREATER, TOKEN_LESS, TOKEN_GREATER_EQ, TOKEN_LESS_EQ,

    TOKEN_BANG,

    TOKEN_TRUE, TOKEN_FALSE, TOKEN_NIL, TOKEN_AND, TOKEN_OR,

    TOKEN_NUM, TOKEN_IDEN, TOKEN_STRING,
    
    TOKEN_NL,
    TOKEN_EOF,

    TOKEN_ERR
} tokenType;

typedef struct {
    tokenType type;
    const char* start;
    int len;
    int line;
} token;

#endif