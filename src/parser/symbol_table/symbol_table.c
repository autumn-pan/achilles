#include ".../ast/ast.c"
#include <stdio.h>
#include <string.h>
#include "./symbol_table.h"
#include "./hash.c"


typedef struct 
{
    unsigned long hash;
    char * identifier;
    enum Modifiers * modifiers;
    char * type;
    char * datatype;
} Symbol;

typedef struct
{
    Symbol * data;
    unsigned long symbol_count;
    unsigned long hash_limit;
} SymbolTable;

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

SymbolTable init_symbol_table()
{
    SymbolTable table = (SymbolTable)malloc(sizeof(SymbolTable));
    table.symbol_count = 0;
    table.data = (Symbol*)malloc(sizeof(Symbol)*table.symbol_count);

    table.hash_limit = 128;

    return table;
}

void append_symbol(SymbolTable table, Symbol symbol)
{
    table.symbol_count++;
    table.data[symbol_count - 1] = symbol;

    // Double hash limit if table runs out of space
    if (table.symbol_count >= table.hash_limit)
    {
        table.hash_limit *= 2;
        table.data = (Symbol*)realloc(sizeof(Symbol) * table.hash_limit);
    }
}

bool insert(SymbolTable table, char * key)
{
    unsigned long hash = hash(key, table.hash_limit)
    
    if (table.data[hash] != NULL)
        return true;

    return false;
}

void delete_symbol(SymbolTable table, char * key)
{
    unsigned long hash = hash(key, table.hash_limit);

    //Free the symbol
    free(table.data[hash - 1]);

    //Clear the symbol
    table.data[hash - 1] = NULL;
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