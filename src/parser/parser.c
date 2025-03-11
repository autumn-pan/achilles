#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../lexer/lexer.c"
#include "../lexer/tokens.c"
#include "../ast/ast.c"

typedef struct {
    int pos;
    int len;
    Token *tokenstream;
} parser;

// Function to initialize a parser
parser *init_parser(Token *tokenstream) {
    parser *p = malloc(sizeof(parser));
    p->pos = 0;
    p->len = 0;
    p->tokenstream = tokenstream;
    return p;
}

void free_parser(parser *p) {
    free(p);
}

// Function to get the current token
Token *get_current_token(parser *p) {
    return &p->tokenstream[p->pos];
}
// Function to get the next token
Token *get_next_token(parser *p) {
    if (p->pos < p->len - 1) {
        Token * val = &p->tokenstream[p->pos];
        p->pos++;
        return val;
    }
    return NULL;
}
// Match function
bool match(parser *p, enum TOKEN_TYPE type) {
    if (p->pos < p->len && p->tokenstream[p->pos].type == type) {
        p->pos++;
        return true;
    }
    return false;
}

// Match value function
bool match_value(parser *p, char *value) {
    if (p->pos < p->len && strcmp(p->tokenstream[p->pos].value, value) == 0) {
        p->pos++;
        return true;
    }
    return false;
}





// If applicable, this function will create a literal node
void parse_literal(parser *parser) {
    Token token = *get_current_token(parser);
    if(match(parser, INT_LITERAL)) {
        createIntNode(token.value);
    }
    else if(match(parser, STR_LITERAL)) {
        createStringNode(token.value);
    }
    else if(match(parser, CHAR_LITERAL)) {
        createCharNode(token.value);
    }
    else if(match(parser, BOOL_LITERAL)) {
        createBoolNode(token.value);
    }
}

// If applicable, this function will consume and create a new AST node that declares a new variable
void parse_variable_declaration(parser *parser) {
    Token token = *get_current_token(parser);

    bool null = false;

    if(!(match_value(parser, "int") || match_value(parser, "string") || match_value(parser, "char") || match_value(parser, "bool")))
        return;
    if(!match(parser, COLON))
        return;
    if(!match(parser, IDENTIFIER))
        return;
    if(!match(parser, SEMI))
    {
        if(!match_value(parser, '='))
            return;
        if(!match(parser, INT_LITERAL) && !match(parser, STR_LITERAL) && !match(parser, CHAR_LITERAL) && !match(parser, BOOL_LITERAL))
            return;
        if(!match(parser, SEMI))
            return;
    }
    switch(token.type) {
        case INT_LITERAL:
            createVariableDeclarationNode(token.value, NULL);
            break;
        case STR_LITERAL:
            createVariableDeclarationNode(token.value, NULL);
            break;
        case CHAR_LITERAL:
            createVariableDeclarationNode(token.value, NULL);
            break;
        case BOOL_LITERAL:
            createVariableDeclarationNode(token.value, NULL);
            break;
        default:
            break;
    }
    ASTNode *node;
    switch(token.type) {
        case INT_LITERAL:
            node = createIntNode(token.value);
            break;
        case STR_LITERAL:
            node = createStringNode(token.value);
            break;
        case CHAR_LITERAL:
            node = createCharNode(token.value);
            break;
        case BOOL_LITERAL:
            node = createBoolNode(token.value);
            break;
        default:
            null = true;
            break;
    }
    createVariableDeclarationNode(token.value, node);
}

