
#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

#include "scanner.h"
#include "../ast.h"

typedef struct {
    scanner scnr;
    token curr;
    ast* nodesToFree;
    bool hadError;
} parser;

void initParser(parser* prsr, const char* src);
void freeParser(parser* prsr);
ast* parse(parser* prsr);
bool parserEOF(parser* prsr);

#endif
