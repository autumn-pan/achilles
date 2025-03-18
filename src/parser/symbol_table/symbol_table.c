#include ".../ast/ast.c"
#include <stdio.h>
#include <string.h>
#include "./symbol_table.h"
#include "./hasher.c"


typedef struct 
{
    unsigned long hash;
    char * identifier;
    enum Modifiers * modifiers;
    char * type;
    char * datatype;
} Symbol;

Symbol init_symbol(ASTNode * node)
{
    char * id = node->data.identifier;
    char * type = type_to_string(node->type);
    char * datatype = NULL;
    char * key;

    if(node->type == VARIABLE_DECL || node->type == FUNCTION_DECL)
        datatype = node->data.IdentifierData.type;

    if(node->type == FUNCTION_DECL)
        key = key_function(node);
    else if (node->type == VARIABLE_DECL)
        key = key_variable(node);

    unsigned long hash = hash(key, hash_limit);

    Symbol symbol = (Symbol)malloc(sizeof(Symbol));
    symbol.identifier = strdup(id);
    symbol.type = strdup(type);
    symbol.datatype = strdup(datatype);
}

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