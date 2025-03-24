#include <stdio.h>
#include <string.h>
#include "./symbol_table.h"
#include "./hash.c"
#include "../../ast/ast.h"
#include <limits.h>

// The symbol struct represents an individual entry in a symbol table
typedef struct Symbol {
    unsigned long hash;
    char * identifier;
    enum Modifiers modifiers;
    char * type;
    char * datatype;
    char * key;
} Symbol;

// This will hold all of the identifiers for a particular scope 
typedef struct
{
    Symbol ** data;

    unsigned long symbol_count;
    unsigned long hash_limit;

} SymbolTable;

// Constructor for a symbol
Symbol * init_symbol(ASTNode * node, unsigned long hash_limit)
{
    char * id = strdup(node->data.IdentifierData->identifier);
    char * type = type_to_string(node->type);
    char * datatype = NULL;
    char * key;

    // Set symbol data type
    if(node->type == VARIABLE_DECL || node->type == FUNCTION_DECL)
        datatype = node->data.IdentifierData->datatype;

    // Evaluate the key of the symbol
    if(node->type == FUNCTION_DECL)
        key = key_function(node);
    else if (node->type == VARIABLE_DECL)
        key = key_variable(node);

    // Construct the symbol
    Symbol *symbol = (Symbol *)malloc(sizeof(Symbol));
    symbol->identifier = strdup(id);
    symbol->type = strdup(type);
    if (datatype != NULL)
        symbol->datatype = strdup(datatype);
    else    
        symbol->datatype = NULL;
    symbol->key = strdup(key);

    return symbol;
}




SymbolTable* init_symbol_table()
{
    SymbolTable* table = (SymbolTable*)malloc(sizeof(SymbolTable));
    table->symbol_count = 0;

    table->hash_limit = 128;
    table->data = (Symbol **)malloc(sizeof(Symbol)*table->hash_limit);

    return table;
}

void insert_symbol(SymbolTable *table, Symbol * symbol) 
{
    table->symbol_count++;

    unsigned long index = symbol->hash;

    while (table->data[index] != NULL) {
        index = (index + 1) % table->hash_limit;
    }

    table->data[index] = symbol;

    // Double hash limit if table runs out of space
    while (table->symbol_count >= table->hash_limit / 2) 
    {
        table->hash_limit *= 2;

        Symbol ** tmp = (Symbol **)malloc(sizeof(Symbol) * table->hash_limit);

        // Rehash all symbols
        for (int i = 0; i < table->hash_limit / 2; i++) 
        {
            if (table->data[i] != NULL) {
                int new_index = hash_key(table->data[i]->key, table->hash_limit);

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
    int index = hash_key(key, table->hash_limit);

    // Resolve collisions via linear probing
    while (table->data[index] != NULL) 
    {
        if (strcmp(table->data[index]->key, key) == 0) {
            return true;
        }
        index = (index + 1) % table->hash_limit;
    }

    // If key is not found,
    return false;
}

unsigned long get_symbol_position(SymbolTable * table, char * key)
{
    unsigned long hash = hash_key(key, table->hash_limit);

    int index = hash_key(key, table->hash_limit);

    while (table->data[index] != NULL) 
    {
        if (strcmp(table->data[index]->key, key) == 0) 
        {
            return index;
        }

        index = (index + 1) % table->hash_limit;
    }

    return ULONG_MAX;
}


void delete_symbol(SymbolTable * table, char * key)
{
    if(!search_symbol(table, key))
        exit(EXIT_FAILURE);

    
    unsigned long index = get_symbol_position(table, key);

    //Free the symbol
    free(table->data[index - 1]);

    //Clear the symbol
    table->data[index - 1] = NULL;
}


char * key_function( ASTNode * node)
{
    char key[256] = "";
    char * params = "(";

    if (node->type == FUNCTION_DECL)
    {
        strcat(key, node->data.IdentifierData->identifier);

        int i = 0;
        while(i < node->numChildren)
        {
            strcat(params, node->children[i]->children[0]->data.IdentifierData->datatype);

            strcat(params, ", ");
        }
        strcat(params, ")");
        strcat(key,params);

        return key;
    }

    return NULL;
}

char * key_variable( ASTNode * node)
{
    char key[256] = "";
    if (node->type == VARIABLE_DECL)
    {
        strcat(key, node->data.IdentifierData->identifier);
        strcat(key, node->children[0]->data.IdentifierData->datatype);

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
        for (unsigned long i = 0; i < table->symbol_count; i++) 
        {
            if (table->data[i] != NULL) 
            {
                free_symbol(table->data[i]); 
            }
        }
        //Free the table itelf
        free(table->data); 
        free(table);      
    }
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Assuming the `Symbol`, `SymbolTable`, and related functions have been defined as in your code
int main() {
    // Initialize the symbol table
    SymbolTable *table = init_symbol_table();

    // Manually define some symbols (assuming keys and attributes are strings for simplicity)
    char *keys[] = {"functionA(int)", "variableX", "functionB(void)", NULL};
    char *types[] = {"FUNCTION", "VARIABLE", "FUNCTION"};
    char *datatypes[] = {"int", "float", "void"};

    // Insert symbols into the table
    for (int i = 0; keys[i] != NULL; i++) {
        Symbol *symbol = (Symbol *)malloc(sizeof(Symbol));
        symbol->key = strdup(keys[i]);
        symbol->type = strdup(types[i]);
        symbol->datatype = strdup(datatypes[i]);
        symbol->identifier = strdup(keys[i]);  // Identifier is same as the key for simplicity
        symbol->hash = hash_key(keys[i], table->hash_limit);

        insert_symbol(table, symbol);  // Insert symbol into the table

        free(symbol);  // Free temporary symbol structure (data is deep copied)
    }

    // Search for a key to test functionality
    char *search_key = "variableX";
    if (search_symbol(table, search_key)) {
        printf("Symbol '%s' found in the table.\n", search_key);
    } else {
        printf("Symbol '%s' not found in the table.\n", search_key);
    }

    // Print all symbols in the table (for debugging purposes)
    printf("All symbols in the table:\n");
    for (unsigned long i = 0; i < table->hash_limit; i++) {
        if (table->data[i] != NULL) {
            printf("Index %lu: Key = %s, Type = %s, Datatype = %s\n",
                   i, table->data[i]->key, table->data[i]->type, table->data[i]->datatype);
        }
    }

    // Free memory
    free_symbol_table(table);
    return 0;
}

