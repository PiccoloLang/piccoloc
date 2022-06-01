
#ifndef RTLIB_QUOTE_H
#define RTLIB_QUOTE_H

val makeQuotedUnary(val value, const char* start, int line, int len, int type);
val makeQuotedBinary(val lhs, val rhs, const char* start, int line, int len, int type);
val makeQuotedQuote(int exprCnt, ...);

#endif
