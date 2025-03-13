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
ASTNode * parse_literal(parser *parser) {
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
ASTNode * parse_variable_declaration(parser *parser) {
    Token token = *get_current_token(parser);

    bool null = false;

    if(!(match_value(parser, "int") || match_value(parser, "string") || match_value(parser, "char") || match_value(parser, "bool")))
        return NULL;
    if(!match(parser, COLON))
        return NULL;
    if(!match(parser, IDENTIFIER))
        return NULL;
    if(!match(parser, SEMI))
    {
        if(!match_value(parser, '='))
            return NULL;
        if(!match(parser, INT_LITERAL) && !match(parser, STR_LITERAL) && !match(parser, CHAR_LITERAL) && !match(parser, BOOL_LITERAL))
            return NULL;
        if(!match(parser, SEMI))
            return NULL;
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

ASTNode * parse_variable_call(parser * parser) 
{
    Token token = *get_current_token(parser);
    if(!match(parser, IDENTIFIER))
        return NULL;
    return createVariableCallNode(token.value);
}

// If applicable, this function will consume and create a new AST node that declares a new function
ASTNode * parse_function_declaration(parser * parser) 
{

    char * name;
    ASTNode * args;
    ASTNode * body;
    if(!match_value(parser, "function" && !match(parser, KEYWORD)))
        return NULL;
    if(!match(parser, IDENTIFIER))
        return NULL;
    
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
            appendNode(args, arg);
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
    return createFunctionDeclarationNode(name, args, body);
}

// If applicable, this function will consume and create a new AST node that declares a new function call
ASTNode * parse_function_call(parser * parser)
{
    Token token = *get_current_token(parser);

    ASTNode * args;

    if(!match(parser, KEYWORD))
        return NULL;
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
            appendNode(args, arg);
        if(!match(parser, COMMA))
            break;
    }
    return createFunctionCallNode(id, args);
}

ASTNode * parse_class_declaration(parser *parser)
{
    Token token = *get_current_token(parser);
    if(!match(parser, KEYWORD) && !match_value(parser, "constructor"))
        return NULL;
    
    if(!match(parser, IDENTIFIER))
        return NULL;

    Token token = *get_current_token(parser);
    char * id = strdup(token.value);

    ASTNode * body = parse_block(parser);
    if(body == NULL)
        return NULL;

    return create_class_declaration_node(id, body);
}

ASTNode * parse_constructor_declaration(parser *parser)
{
    if(!match(parser, KEYWORD) && !match_value(parser, "constructor"))
        return NULL;
    if(!match(parser, LPAR))
        return NULL;
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
    return createConstructorDeclarationNode(args, body);
}

ASTNode * parse_block(parser * parser)
{
    ASTNode * body;
    if(!match(parser, LBRACE))
        return NULL;
    while(!match(parser, RBRACE))
    {
        ASTNode * stmt = parse_statement(parser);
        if(stmt == NULL)
            return NULL;
        if(body == NULL)
            body = stmt;
        else
            appendNode(body, stmt);
    }
    return createBlockNode(body);
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
        return parse_class_declaration;
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
    if(!match(parser, KEYWORD && !match_value(parser, "if")))
        return NULL;
    if(!match(parser, LPAR))
        return NULL;
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
    if(!match(parser, KEYWORD) && !match_value(parser, "else"))
        return NULL;
    if(!match(parser, KEYWORD) && !match_value(parser, "if"))
        return NULL;
    if(!match(parser, LPAR))
        return NULL;

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
    if(!match(parser, KEYWORD) && !match_value(parser, "else"))
        return NULL;
    ASTNode * body = parse_block(parser);
    if(body == NULL)
        return NULL;
    return create_else_node(body);
}

ASTNode * parse_expression(parser * parser)
{
    ASTNode * left = parse_term(parser);
    ASTNode * node = left;

    while(!match(parser, RPAR) && !match(parser, SEMI) && !match(parser, EOL))
    {
        Token token = *get_current_token(parser);
        ASTNode * op = createOperatorNode(token.value);
        ASTNode * right = parse_term(parser);
        if(right == NULL)
            return NULL;
        ASTNode * newnode = createBinaryOperationNode(op, left, right);
        node = newnode;
    }

    return node;
}


ASTNode * parse_term(parser * parser)
{
    ASTNode * node = parse_factor(parser);

    while(match(parser, OPERATOR))
    {
        Token token = *get_current_token(parser);
        ASTNode * op = createOperatorNode(token.value);
        ASTNode * right = parse_factor(parser);
        if(right == NULL)
            return NULL;
        ASTNode * newnode = createBinaryOperationNode(op, node, right);
        node = newnode;
    }
    return node;
}


ASTNode * parse_factor(parser * parser)
{
    if(match(parser, LPAR))
    {
        parse_expression(parser);
    }
    
    else if(parse_literal(parser) != NULL)
        return parse_literal(parser);
    else if(parse_variable_call(parser) != NULL)
        return parse_variable_call(parser);
    else if(parse_function_call(parser) != NULL)
        return parse_function_call(parser);
    else
        return NULL;

}
