#include <stdio.h>
#include <string.h>
#include "../../ast/ast.c"

typedef enum Modifiers {
    PUBLIC,
    PRIVATE,
    STATIC,
    NON_STATIC,
    MUTABLE,
    CONST
};

char * key_function( ASTNode * node);
char * key_variable( ASTNode * node);
