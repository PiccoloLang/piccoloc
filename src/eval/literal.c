
#include "../eval.h"

val evalLiteral(engine* engine, astLiteral* literal) {
    (void)engine;
    if(literal->useValue)
        return literal->value;
    return NIL_VAL(); // TODO: make this create the literal value from the token. this is only really needed for compile time eval
}
