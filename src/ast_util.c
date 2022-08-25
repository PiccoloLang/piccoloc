
#include "ast_util.h"

#include <stdlib.h>

int astNextChainLen(ast* ast) {
    if(ast == NULL)
        return 0;
    return 1 + astNextChainLen(ast->next);
}
