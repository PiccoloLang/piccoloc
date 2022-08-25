
#ifndef QUOTE_H
#define QUOTE_H

val makeQuotedBlockFromArr(int exprCnt, val* elems);
val makeQuotedBlock(int exprCnt, ...);
val makeQuotedCallFromArr(val fn, const char* tknStart, int tknLine, int tknLen, int tknType, int argCnt, val* args);
val makeQuotedCall(val fn, const char* tknStart, int tknLine, int tknLen, int tknType, int argCnt, ...);
val makeQuotedQuote(int exprCnt, ...);
val makeQuotedFnFromArr(val body, int argCnt, val* args);
val makeQuotedFn(val body, int argCnt, ...);

#include "quote_gen.h"

#endif
