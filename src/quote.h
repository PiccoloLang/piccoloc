
#ifndef QUOTE_H
#define QUOTE_H

val makeQuotedBlockFromArr(int exprCnt, val* elems);
val makeQuotedBlock(int exprCnt, ...);
val makeQuotedQuote(int exprCnt, ...);

#include "quote_gen.h"

#endif
