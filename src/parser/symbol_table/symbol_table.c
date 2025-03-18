#include ".../ast/ast.c"
#include <stdio.h>
#include <string.h>
#include "./symbol_table.h"

char * key_function(ASTNode * node)
{
    char * key = "";
    char * params = "(";

    if (node->type == FUNCTION_DECL)
    {
        strcat(key, node->data.identifier);

        int i = 0;
        while(i < node->numChildren)
        {
            strcat(params, node->children[i]->children[0]->type);

            strcat(params, ", ");
        }
        strcat(params, ")");
        strcat(key,params);

        return key;
    }

    return NULL;
}

char * key_variable(ASTNode * node)
{
    char * key = "";
    if (node->type == VARIABLE_DECL)
    {
        strcat(key, node->data.identifier);
        strcat(key, node->children[0]->type);

        return key;
    }
    return NULL;
}