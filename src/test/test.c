
#include "../value.h"
#include "../token.h"
#include "../rtlib/quote.h"
#include "../ast.h"

extern void print(val value);

astUnary* aaa(val x) {
    return AS_OBJ(x);
}

int main() {
    val x = NUM_VAL(5);
    val u1 = makeQuotedUnary(x, "-", 1, 1, TOKEN_MINUS);
    astUnary* a1 = aaa(u1);
    val u2 = makeQuotedUnary(u1, "-", 1, 1, TOKEN_MINUS);
    astUnary* a2 = aaa(u2);
    val u3 = makeQuotedUnary(u2, "-", 1, 1, TOKEN_MINUS);
    astUnary* a3 = aaa(u3);
    val u4 = makeQuotedUnary(u3, "-", 1, 1, TOKEN_MINUS);
    astUnary* a4 = aaa(u4);

    print(u4);

}