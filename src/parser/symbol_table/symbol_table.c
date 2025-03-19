#include ".../ast/ast.c"
#include <stdio.h>
#include <string.h>
#include "./symbol_table.h"
#include "./hash.c"

// The symbol struct represents an individual entry in a symbol table
typedef struct 
{
    unsigned long hash;
    char * identifier;
    enum Modifiers * modifiers;
    char * type;
    char * datatype;
    char * key;
} Symbol;

// This will hold all of the identifiers for a particular scope 
typedef struct
{
    Symbol * data;
    unsigned long symbol_count;
    unsigned long hash_limit;
} SymbolTable;

// Constructor for a symbol
Symbol init_symbol(ASTNode * node)
{
    char * id = node->data.identifier;
    char * type = type_to_string(node->type);
    char * datatype = NULL;
    char * key;

    // Set symbol data type
    if(node->type == VARIABLE_DECL || node->type == FUNCTION_DECL)
        datatype = node->data.IdentifierData.type;

    // Evaluate the key of the symbol
    if(node->type == FUNCTION_DECL)
        key = key_function(node);
    else if (node->type == VARIABLE_DECL)
        key = key_variable(node);

    // Evaluate the hash
    unsigned long hash = hash(key, hash_limit);

    // Construct the symbol
    Symbol symbol = (Symbol)malloc(sizeof(Symbol));
    symbol.identifier = strdup(id);
    symbol.type = strdup(type);
    symbol.datatype = strdup(datatype);
    symbol.key = strdup(key);

    return symbol;
}



SymbolTable* init_symbol_table()
{
    SymbolTable* table = (SymbolTable*)malloc(sizeof(SymbolTable));
    table->symbol_count = 0;
    table->data = (Symbol*)malloc(sizeof(Symbol)*table.symbol_count);

    table->hash_limit = 128;

    return table;
}

void insert_symbol(SymbolTable *table, Symbol symbol) {
    table->symbol_count++;
    table->data[table->symbol_count - 1] = symbol;

    // Double hash limit if table runs out of space
    while (table->symbol_count >= table->hash_limit / 2) {
        table->hash_limit *= 2;

        Symbol *tmp = (Symbol *)malloc(sizeof(Symbol) * table->hash_limit);

        // Rehash all symbols
        for (int i = 0; i < table->hash_limit / 2; i++) {
            if (table->data[i] != NULL) {
                int new_index = hash(table->data[i].key, table->hash_limit);

                // Handle collisions by linear probing
                while (tmp[new_index] != NULL) {
                    new_index = (new_index + 1) % table->hash_limit;
                }

                tmp[new_index] = table->data[i];
            }
        }

        free(table->data);
        table->data = tmp;
    }
}

bool search_symbol(SymbolTable *table, char *key) {
    int index = hash(key, table->hash_limit);

    // Resolve collisions via linear probing
    while (table->data[index] != NULL) 
    {
        if (strcmp(table->data[index]->key, key) == 0) {
            return true
        }
        index = (index + 1) % table->hash_limit;
    }

    // If key is not found,
    return false;
}
unsigned long get_symbol_position(SymbolTable * table, char * key)
{
    unsigned long hash = hash(key, table->hash_limit);

    int index = hash(key, table->hash_limit);

    while (table->data[index] != NULL) 
    {
        if (strcmp(table->data[index]->key, key) == 0) 
        {
            return index;
        }

        index = (index + 1) % table->hash_limit;
    }NULL

    return NULL;
}


void delete_symbol(SymbolTable table, char * key)
{
    if(!search_symbol(table, key))
        exit(EXIT_FAILURE);

    
    unsigned long index = get_symbol_position(table, key)

    //Free the symbol
    free(table->data[index - 1]);

    //Clear the symbol
    table->data[index - 1] = NULL;
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


void free_symbol(Symbol *symbol) {
    if (symbol) {
        free(symbol->identifier);
        free(symbol->type);
        free(symbol->datatype);
        free(symbol->key);
        free(symbol);
    }
}

void free_symbol_table(SymbolTable *table) {
    if (table) {
        // Free every symbol in the table
        for (unsigned long i = 0; i < table->symbol_count; i++) {
            if (table->data[i]) {
                free_symbol(&(table->data[i])); 
            }
        }
        //Free the table itelf
        free(table->data); 
        free(table);      
    }
}