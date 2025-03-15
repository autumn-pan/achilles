#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../lexer/lexer.c"
#include "../lexer/tokens.c"
#include "../ast/ast.c"
#include "../error_handling.c"

typedef struct {
    int pos;
    int len;
    Token *tokenstream;
    ErrorList *errorList;
} parser;

// Function to initialize a parser
parser *init_parser(Token *tokenstream) {
    parser *p = malloc(sizeof(parser));
    p->pos = 0;
    p->len = 0;
    p->tokenstream = tokenstream;
    p->errorList = create_error_list();
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

void parser_error(parser *p, const char *message, ErrorList *list) {
    Error *error = create_error(p->tokenstream[p->pos].line, p->tokenstream[p->pos].column, message);
    add_error(list, error);
    fprintf(stderr, "Parser error at line %d, column %d: %s\n", p->tokenstream[p->pos].line, p->tokenstream[p->pos].column, message);
     // Free the parser and exit
    free_parser(p);
     // Free the error list
    free_error_list(list);
    // Exit the program
    exit(EXIT_FAILURE);
}

// Match function
// Returns true if the current token matches the given type, also advances the parser by one token
bool match(parser *p, enum TOKEN_TYPE type) {
    if (p->pos < p->len && p->tokenstream[p->pos].type == type) {
        p->pos++;
        return true;
    }
    parser_error(p, "Unexpected token type", p->errorList);
    return false;
}

// Match value function
bool match_value(parser *p, char *value) {
    if (p->pos < p->len && strcmp(p->tokenstream[p->pos].value, value) == 0) {
        p->pos++;
        return true;
    }
    parser_error(p, "Unexpected token value", p->errorList);
    return false;
}

// If applicable, this function will create a literal node
ASTNode * parse_literal(parser *parser) {
    Token token = *get_current_token(parser);

    if(match(parser, INT_LITERAL)) {
        return create_int_node(token.value);
    }
    else if(match(parser, STR_LITERAL)) {
        return create_string_node(token.value);
    }
    else if(match(parser, CHAR_LITERAL)) {
        return create_char_node(token.value);
    }
    else if(match(parser, BOOL_LITERAL)) {
        return create_boolean_node(token.value);
    }
    parser_error(parser, "Expected a literal", parser->errorList);
    return NULL;
}

// If applicable, this function will consume and create a new AST node that declares a new variable
ASTNode * parse_variable_declaration(parser *parser) {
    Token token = *get_current_token(parser);

    bool null = false;

    if(!(match_value(parser, "int") || match_value(parser, "string") || match_value(parser, "char") || match_value(parser, "bool")))
        parser_error(parser, "Expected a type keyword", parser->errorList);
    if(!match(parser, COLON))
        parser_error(parser, "Expected ':'", parser->errorList);
    if(!match(parser, IDENTIFIER))
        parser_error(parser, "Expected an identifier", parser->errorList);
    if(!match(parser, SEMI))
    {
        if(!match_value(parser, "="))
            parser_error(parser, "Expected '='", parser->errorList);
        if(!match(parser, INT_LITERAL) && !match(parser, STR_LITERAL) && !match(parser, CHAR_LITERAL) && !match(parser, BOOL_LITERAL))
            parser_error(parser, "Expected a literal", parser->errorList);
        if(!match(parser, SEMI))
            parser_error(parser, "Expected ';'", parser->errorList);
    }
    switch(token.type) {
        case INT_LITERAL:
            create_variable_declaration_node(token.value, NULL);
            break;
        case STR_LITERAL:
            create_variable_declaration_node(token.value, NULL);
            break;
        case CHAR_LITERAL:
            create_variable_declaration_node(token.value, NULL);
            break;
        case BOOL_LITERAL:
            create_variable_declaration_node(token.value, NULL);
            break;
        default:
            break;
    }
    ASTNode *node;
    switch(token.type) {
        case INT_LITERAL:
            node = create_int_node(token.value);
            break;
        case STR_LITERAL:
            node = create_string_node(token.value);
            break;
        case CHAR_LITERAL:
            node = create_char_node(token.value);
            break;
        case BOOL_LITERAL:
            node = create_bool_node(token.value);
            break;
        default:
            null = true;
            break;
    }
    return create_variable_declaration_node(token.value, node);
}

ASTNode * parse_variable_call(parser * parser) 
{
    Token token = *get_current_token(parser);
    if(!match(parser, IDENTIFIER))
        return NULL;
    return create_variable_call_node(token.value);
}

// If applicable, this function will consume and create a new AST node that declares a new function
ASTNode * parse_function_declaration(parser * parser) 
{

    char * name;
    ASTNode * args;
    ASTNode * body;
    if(!match_value(parser, "function") || !match(parser, KEYWORD))
        parser_error(parser, "Expected 'function' keyword", parser->errorList);
    if(!match(parser, IDENTIFIER))
        parser_error(parser, "Expected function name", parser->errorList);
    
    Token token = *get_current_token(parser);

    name = strdup(token.value);
    if(!match(parser, LPAR))
        return NULL;
    while(!match(parser, RPAR) && !match(parser, SEMI) && !match(parser, EOL))
     {
        ASTNode * arg = parse_variable_declaration(parser);
        if(arg == NULL)
            return NULL;
        if(args == NULL)
            args = arg;
        else
            append_node(args, arg);
        if(!match(parser, COMMA))
            break;
    }
    if(!match(parser, LBRACE))
        return NULL;
    while(!match(parser, RBRACE))
    {
        ASTNode * statement = parse_statement(parser);
        if(statement == NULL)
            return NULL;
        if(body == NULL)
            body = statement;
        else
            appendNode(body, statement);
    }
    return create_function_declaration_node(name, args, body);
}

// If applicable, this function will consume and create a new AST node that declares a new function call
ASTNode * parse_function_call(parser * parser)
{
    Token token = *get_current_token(parser);

    ASTNode * args;

    if(!match(parser, KEYWORD))
        parser_error(parser, "Expected function call keyword", parser->errorList);
    char * id = strdup(token.value);
    if(!match(parser, LPAR))
        return NULL;

    while(!match(parser, RPAR))
    {
        ASTNode * arg;
        if(parse_literal(parser) != NULL)
            arg = parse_literal(parser);
        else if(parse_variable_call(parser) != NULL)
            arg = parse_variable_call(parser);
        else if(parse_function_call(parser) != NULL)
            arg = parse_function_call(parser);
        else
            return NULL;


        if(arg == NULL)
            return NULL;
        if(args == NULL)
            args = arg;
        else
            append_node(args, arg);
        if(!match(parser, COMMA))
            break;
    }
    return create_function_call_node(id, args);
}

ASTNode * parse_class_declaration(parser *parser)
{
    Token token = *get_current_token(parser);
    if(!match(parser, KEYWORD) || !match_value(parser, "class"))
        parser_error(parser, "Expected 'class' keyword", parser->errorList);
    
    if(!match(parser, IDENTIFIER))
        parser_error(parser, "Expected class name", parser->errorList);

    token = *get_current_token(parser);
    char * id = strdup(token.value);

    ASTNode * body = parse_block(parser);
    if(body == NULL)
        return NULL;

    return create_class_declaration_node(id, body);
}

ASTNode * parse_constructor_declaration(parser *parser)
{
    if(!match(parser, KEYWORD) || !match_value(parser, "constructor"))
        parser_error(parser, "Expected 'constructor' keyword", parser->errorList);
    if(!match(parser, LPAR))
        parser_error(parser, "Expected '('", parser->errorList);
    ASTNode * args;
    while(!match(parser, RPAR))
    {
        ASTNode * arg = parse_variable_declaration(parser);

        if(arg == NULL)
            return NULL;
        if(args == NULL)
            args = arg;
        else
            appendNode(args, arg);
    }

    ASTNode * body = parse_block(parser);
    if(body == NULL)
        return NULL;
    return create_constructor_declaration_node(args, body);
}

ASTNode * parse_block(parser * parser)
{
    ASTNode * body;
    int numChildren = 0;

    if(!match(parser, LBRACE))
        parser_error(parser, "Expected '{'", parser->errorList);
    while(!match(parser, RBRACE))
    {
        ASTNode * stmt = parse_statement(parser);


        if(stmt == NULL)
            return NULL;
        if(body == NULL)
            body = stmt;
        else
        {
            append_node(body, stmt);
            numChildren++;
        }
    }
    return create_block_node(body, numChildren);
}

ASTNode * parse_statement(parser * parser) 
{
    if(parse_variable_declaration(parser) != NULL)
        return parse_variable_declaration(parser);
    else if(parse_function_declaration(parser) != NULL)
        return parse_function_declaration(parser);
    else if(parse_function_call(parser) != NULL)
        return parse_function_call(parser);
    else if(parse_class_declaration(parser) != NULL)
        return parse_class_declaration(parser);
    else if(parse_constructor_declaration(parser) != NULL)
        return parse_constructor_declaration(parser);
    else if(parse_block(parser) != NULL)
        return parse_block(parser);
    else if(parse_literal(parser) != NULL)
        return parse_literal(parser);
    else if(parse_variable_call(parser) != NULL)
        return parse_variable_call(parser);
    else
        return NULL;
}


ASTNode * parse_if(parser * parser)
{
    if(!match(parser, KEYWORD) || !match_value(parser, "if"))
        parser_error(parser, "Expected 'if' keyword", parser->errorList);
    if(!match(parser, LPAR))
        parser_error(parser, "Expected '('", parser->errorList);
    ASTNode * condition = parse_expression(parser);
    if(condition == NULL)
        return NULL;

    ASTNode * body = parse_block(parser);
    if(body == NULL)
        return NULL;
    
    return create_if_node(condition, body);
}

ASTNode * parse_else_if(parser *parser)
{
    if(!match(parser, KEYWORD) || !match_value(parser, "else"))
        parser_error(parser, "Expected 'else' keyword", parser->errorList);
    if(!match(parser, KEYWORD) || !match_value(parser, "if"))
        parser_error(parser, "Expected 'if' keyword", parser->errorList);
    if(!match(parser, LPAR))
        parser_error(parser, "Expected '('", parser->errorList);

    ASTNode * condition = parse_expression(parser);

    if(condition == NULL)
        return NULL;
    
    ASTNode * body = parse_block(parser);
    if(body == NULL)
        return NULL;

    return create_else_if_node(condition, body);
}

ASTNode * parse_else(parser *parser)
{
    if(!match(parser, KEYWORD) || !match_value(parser, "else"))
        parser_error(parser, "Expected 'else' keyword", parser->errorList);
    ASTNode * body = parse_block(parser);
    if(body == NULL)
        return NULL;
    return create_else_node(body);
}
// Parses the expression
ASTNode * parse_expression(parser * parser)
{
    ASTNode * left = parse_term(parser);
    ASTNode * node = left;

    while(!match(parser, RPAR) && !match(parser, SEMI) && !match(parser, EOL))
    {
        Token token = *get_current_token(parser);
        
        ASTNode * right = parse_term(parser);
        if(right == NULL)
            return NULL;
        ASTNode * newnode = create_binary_operator_node(token.value, left, right);
        node = newnode;
    }

    return node;
}

// Parses each term in the expression
ASTNode * parse_term(parser * parser)
{
    ASTNode * node = parse_factor(parser);

    while(match(parser, OPERATOR))
    {
        Token token = *get_current_token(parser);
        ASTNode * right = parse_factor(parser);
        if(right == NULL)
            return NULL;
            
        ASTNode * newnode = create_binary_operator_node(token.value, node, right);
        node = newnode;
    }
    return node;
}


ASTNode * parse_factor(parser * parser)
{
    if(match(parser, LPAR))
    {
        return parse_expression(parser);
    }
    
    else if(parse_literal(parser) != NULL)
        return parse_literal(parser);
    else if(parse_variable_call(parser) != NULL)
        return parse_variable_call(parser);
    else if(parse_function_call(parser) != NULL)
        return parse_function_call(parser);
    parser_error(parser, "Expected a factor", parser->errorList);
    return NULL;

}

ASTNode * parse_while_loop(parser * parser)
{
    if(!match(parser, KEYWORD) || !match_value(parser, "while"))
        parser_error(parser, "Expected 'while' keyword", parser->errorList);
    if(!match(parser, LPAR))
        parser_error(parser, "Expected '('", parser->errorList);
    ASTNode * condition = parse_expression(parser);
    if(condition == NULL)
        return NULL;
    ASTNode * body = parse_block(parser);
    if(body == NULL)
        return NULL;
    return create_while_loop_node(condition, body);
}


ASTNode * parse_for_loop(parser * parser)
{
    if (!match(parser, KEYWORD) || !match_value(parser, "for"))
        parser_error(parser, "Expected 'for' keyword", parser->errorList);
    if (!match(parser, LPAR))
        parser_error(parser, "Expected '('", parser->errorList);
    ASTNode * init = parse_variable_declaration(parser);
    if (init == NULL)
        return NULL;
    
    ASTNode * condition = parse_expression(parser);
    if (condition == NULL)
        return NULL;
    
    ASTNode * update = parse_variable_declaration(parser);
    if (update == NULL)
        return NULL;
    
    ASTNode * body = parse_block(parser);
    if (body == NULL)
        return NULL;

    return create_for_loop_node(init, condition, update, body);
}

ASTNode * parse_return(parser * parser)
{
    if(!match(parser, KEYWORD) || !match_value(parser, "return"))
        parser_error(parser, "Expected 'return' keyword", parser->errorList);
    ASTNode * value = parse_expression(parser);
    if(value == NULL)
        return NULL;
    return create_return_node(value);
}

