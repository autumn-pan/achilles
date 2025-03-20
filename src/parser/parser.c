#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "src/parser/lexer/lexer.h"
#include "src/parser/lexer/tokens.c"
#include "src/ast/ast.h"
#include "src/error_handling.h"

typedef struct parser {
    int pos;
    int len;
    Token *tokenstream;
    struct ErrorList *errorList;
} parser;

// Function to initialize a parser
parser *init_parser(Token *tokenstream) {
    parser *p = malloc(sizeof(parser));
    p->pos = 0;
    p->len = 0;
    while (tokenstream[p->len].type != END_OF_FILE) {
        p->len++;
    }
    p->tokenstream = tokenstream;
    p->errorList = create_error_list();
    return p;
}
parser *init_parser(Token *tokenstream) {
    parser *p = malloc(sizeof(parser));
    p->pos = 0;
    p->len = 0;
    while (tokenstream[p->len].type != END_OF_FILE) {
        p->len++;
    }
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

bool is_end_of_line(Token *token) {
    return token->type == EOL || token->type == END_OF_FILE || token->type == SEMI;
}
Token * advance_parser(parser *p) {
    return &p->tokenstream[p->pos++];
}
const char * END_OF_TERM_OPS[] = {
    ";",
    ",",
    ".",
    "}",
    "]",
    ")",
    "+",
    "-",
    "==",
    "<",
    ">",
    "<=",
    ">=",
    "+=",
    "-=",
    "*=",
    "/=",
    "="
};

bool is_end_of_term(Token *token) {
    if(is_end_of_line(token))
        return true;
    for(int i = 0; i < sizeof(END_OF_TERM_OPS) / sizeof(END_OF_TERM_OPS[0]); i++)
    {
        if(strcmp(token->value, END_OF_TERM_OPS[i]) == 0)
            return true;
    }
    return false;
}

// Function to get the next token
Token *get_next_token(parser *p) {
    if (p->pos < p->len) {
        return &p->tokenstream[p->pos++];
    }
    return NULL;
}

void parser_error(struct parser *p, const char *message, struct ErrorList *list) {
    struct Error *error = create_error(p->tokenstream[p->pos].line, p->tokenstream[p->pos].column, message);
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
struct ASTNode * parse_literal(parser *parser) {
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
struct ASTNode * parse_variable_declaration(parser *parser) {
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
    struct ASTNode *node = NULL;
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
            break;
    }
    return create_variable_declaration_node(token.value, node);
}

struct ASTNode * parse_variable_call(parser * parser) 
{
    Token token = *get_current_token(parser);
    if(!match(parser, IDENTIFIER))
        return NULL;
    return create_variable_call_node(token.value);
}

// If applicable, this function will consume and create a new AST node that declares a new function
struct ASTNode * parse_function_declaration(parser * parser) 
{

    char * name;
    struct ASTNode * args = NULL;
    struct ASTNode * body = NULL;
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
        struct ASTNode * arg = parse_variable_declaration(parser);
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
        struct ASTNode * statement = parse_statement(parser);
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
struct ASTNode * parse_function_call(parser * parser)
{
    Token token = *get_current_token(parser);
    struct ASTNode * args = NULL;

    if(!match(parser, KEYWORD))
        parser_error(parser, "Expected function call keyword", parser->errorList);
    char * id = strdup(token.value);
    if(!match(parser, LPAR))
        return NULL;

    while(!match(parser, RPAR))
    {
        struct ASTNode * arg;
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

struct ASTNode * parse_class_declaration(parser *parser)
{
    Token token = *get_current_token(parser);
    if(!match(parser, KEYWORD) || !match_value(parser, "class"))
        parser_error(parser, "Expected 'class' keyword", parser->errorList);
    
    if(!match(parser, IDENTIFIER))
        parser_error(parser, "Expected class name", parser->errorList);

    token = *get_current_token(parser);
    char * id = strdup(token.value);

    struct ASTNode * body = parse_block(parser);
    if(body == NULL)
        return NULL;

    return create_class_declaration_node(id, body);
}

struct ASTNode * parse_constructor_declaration(parser *parser)
{
    struct ASTNode * args = NULL;
    if(!match(parser, KEYWORD) || !match_value(parser, "constructor"))
        parser_error(parser, "Expected 'constructor' keyword", parser->errorList);
    if(!match(parser, LPAR))
        parser_error(parser, "Expected '('", parser->errorList);
    while(!match(parser, RPAR))
    {
        struct ASTNode * arg = parse_variable_declaration(parser);

        if(arg == NULL)
            return NULL;
        if(args == NULL)
            args = arg;
        else
            appendNode(args, arg);
    }

    struct ASTNode * body = parse_block(parser);
    if(body == NULL)
        return NULL;
    return create_constructor_declaration_node(args, body);
}

struct ASTNode * parse_block(parser * parser)
{
    struct ASTNode * body = NULL;
    int numChildren = 0;

    if(!match(parser, LBRACE))
        parser_error(parser, "Expected '{'", parser->errorList);
    while(!match(parser, RBRACE))
    {
        struct ASTNode * stmt = parse_statement(parser);


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

struct ASTNode * parse_statement(parser * parser) 
{
    struct ASTNode *node = NULL;
    if((node = parse_variable_declaration(parser)) != NULL)
        return node;
    else if((node = parse_function_declaration(parser)) != NULL)
        return node;
    else if((node = parse_function_call(parser)) != NULL)
        return node;
    else if((node = parse_class_declaration(parser)) != NULL)
        return node;
    else if((node = parse_constructor_declaration(parser)) != NULL)
        return node;
    else if((node = parse_block(parser)) != NULL)
        return node;
    else if((node = parse_literal(parser)) != NULL)
        return node;
    else if((node = parse_variable_call(parser)) != NULL)
        return node;
    else
        return NULL;
}


struct ASTNode * parse_if(parser * parser)
{
    if(!match(parser, KEYWORD) || !match_value(parser, "if"))
        parser_error(parser, "Expected 'if' keyword", parser->errorList);
    if(!match(parser, LPAR))
        parser_error(parser, "Expected '('", parser->errorList);
    struct ASTNode * condition = parse_expression(parser);
    if(condition == NULL)
        return NULL;

    struct ASTNode * body = parse_block(parser);
    if(body == NULL)
        return NULL;
    
    return create_if_node(condition, body);
}

struct ASTNode * parse_else_if(parser *parser)
{
    if(!match(parser, KEYWORD) || !match_value(parser, "else"))
        parser_error(parser, "Expected 'else' keyword", parser->errorList);
    if(!match(parser, KEYWORD) || !match_value(parser, "if"))
        parser_error(parser, "Expected 'if' keyword", parser->errorList);
    if(!match(parser, LPAR))
        parser_error(parser, "Expected '('", parser->errorList);

    struct ASTNode * condition = parse_expression(parser);

    if(condition == NULL)
        return NULL;
    
    struct ASTNode * body = parse_block(parser);
    if(body == NULL)
        return NULL;

    return create_else_if_node(condition, body);
}

struct ASTNode * parse_else(parser *parser)
{
    if(!match(parser, KEYWORD) || !match_value(parser, "else"))
        parser_error(parser, "Expected 'else' keyword", parser->errorList);
    struct ASTNode * body = parse_block(parser);
    if(body == NULL)
        return NULL;
    return create_else_node(body);
}
// Parses the expression
struct ASTNode * parse_expression(parser * parser) {
    struct ASTNode * left = parse_term(parser);
    if (left == NULL) return NULL;

    while (!is_end_of_term(get_current_token(parser))) {
        Token token = *get_current_token(parser);
        if (token.type != OPERATOR) break;

        advance_parser(parser);
        struct ASTNode * right = parse_term(parser);
        if (right == NULL) return NULL;

        left = create_binary_operator_node(token.value, left, right);
    }

    return left;
}

// Parses each term in the expression
struct ASTNode * parse_term(parser * parser) {
    struct ASTNode * node = parse_factor(parser);
    if (node == NULL) return NULL;

    while (!is_end_of_term(get_current_token(parser))) {
        Token token = *get_current_token(parser);
        if (token.type != OPERATOR) break;

        advance_parser(parser);
        struct ASTNode * right = parse_factor(parser);
        if (right == NULL) return NULL;

        node = create_binary_operator_node(token.value, node, right);
    }

    return node;
}


struct ASTNode * parse_factor(parser * parser) {
    if (match(parser, LPAR)) {
        struct ASTNode * expr = parse_expression(parser);
        if (!match(parser, RPAR)) {
            parser_error(parser, "Expected ')'", parser->errorList);
            return NULL;
        }
        return expr;
    }

    struct ASTNode * node = NULL;
    if ((node = parse_literal(parser)) != NULL) return node;
    if ((node = parse_variable_call(parser)) != NULL) return node;
    if ((node = parse_function_call(parser)) != NULL) return node;

    parser_error(parser, "Expected a factor", parser->errorList);
    return NULL;
}

struct ASTNode * parse_while_loop(parser * parser)
{
    if(!match(parser, KEYWORD) || !match_value(parser, "while"))
        parser_error(parser, "Expected 'while' keyword", parser->errorList);
    if(!match(parser, LPAR))
        parser_error(parser, "Expected '('", parser->errorList);
    struct ASTNode * condition = parse_expression(parser);
    if(condition == NULL)
        return NULL;
    struct ASTNode * body = parse_block(parser);
    if(body == NULL)
        return NULL;
    return create_while_loop_node(condition, body);
}


struct ASTNode * parse_for_loop(parser * parser)
{
    if (!match(parser, KEYWORD) || !match_value(parser, "for"))
        parser_error(parser, "Expected 'for' keyword", parser->errorList);
    if (!match(parser, LPAR))
        parser_error(parser, "Expected '('", parser->errorList);
    struct ASTNode * init = parse_variable_declaration(parser);
    if (init == NULL)
        return NULL;
    
    struct ASTNode * condition = parse_expression(parser);
    if (condition == NULL)
        return NULL;
    
    struct ASTNode * update = parse_variable_declaration(parser);
    if (update == NULL)
        return NULL;
    
    struct ASTNode * body = parse_block(parser);
    if (body == NULL)
        return NULL;

    return create_for_loop_node(init, condition, update, body);
}

struct ASTNode * parse_return(parser * parser)
{
    if(!match(parser, KEYWORD) || !match_value(parser, "return"))
        parser_error(parser, "Expected 'return' keyword", parser->errorList);
    struct ASTNode * value = parse_expression(parser);
    if(value == NULL)
        return NULL;
    return create_return_node(value);
}

struct ASTNode * parse(parser * parser)
{
    struct ASTNode *root = NULL;
    while (parser->pos < parser->len) {
        struct ASTNode *stmt = parse_statement(parser);
        if (stmt == NULL) {
            free_parser(parser);
            free_error_list(parser->errorList);
            return NULL;
        }
        if (root == NULL)
            root = stmt;
        else
            append_node(root, stmt);
    }

    return root;
}

void free_parser_resources(parser *p) {
    free_parser(p);
    free_error_list(p->errorList);
}

int main()
{
    TokenStream * ts = tokenize();

    parser *p = init_parser(ts->token_stream);

    struct ASTNode *ast = parse(p);

    if (ast != NULL) {
        printf("Parsing successful!\n");
    } else {
        printf("Parsing failed!\n");
    }
}