
#ifndef QUOTE_H
#define QUOTE_H

val makeQuotedUnary(val value, const char* start, int line, int len, int type);
val makeQuotedBinary(val lhs, val rhs, const char* start, int line, int len, int type);
val makeQuotedQuote(int exprCnt, ...);
val makeQuotedEval(val expr);

#endif
