
#ifndef LEXER_H
#define LEXER_H

#include "../token.h"

typedef struct {
    const char* src;
    const char* start;
    const char* curr;
    int line;
} scanner;

void initScanner(scanner* scnr, const char* src);
token scanToken(scanner* scnr);

#endif
