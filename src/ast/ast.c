#include <stdio.h>
#include <stdlib.h>
#include "../parser/lexer/lexer.h"
#include "../parser/lexer/tokens.c"
#include "./ast.h"
// Define the structure of an AST node
typedef struct {
    char * identifier;
    char * datatype;
} IdentifierData;

typedef struct {
    NodeType type;
    ASTNode **children;

    int numChildren;

    union {
        int intval;
        float floatval;
        char *strval;
        char charval;
        IdentifierData IdentifierData;
        char *operator;
    } data;
} ASTNode;


IdentifierData init_identifier(char *id, char*type)
{
    IdentifierData data = (IdentifierData)malloc(sizeof(ASTNode));
    data.identifier = strdup(id);
    data.datatype = strdup(type);
}

void free_identifier(IdentifierData id)
{
    free(id.identifier);
    free(id.datatype);
}

ASTNode* create_int_node(int num) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = INT;
    node->data.intval = num;
    return node;
}

ASTNode* create_float_node(int num) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = FLOAT;
    node->data.floatval = num;
    return node;
}

ASTNode* create_char_node(char c) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = CHAR;
    node->data.charval = c;
    return node;
}

ASTNode* create_string_node(char * str) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = STRING;
    node->data.strval = strdup(str);
    return node;
}

ASTNode* create_boolean_node(int boolean) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = BOOL;
    node->data.intval = boolean;
    return node;
}

ASTNode* create_binary_operator_node(NodeType op, ASTNode *left, ASTNode *right) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = op;
    node->numChildren = 2;
    node->children = (ASTNode**)malloc(sizeof(ASTNode*) * 2);

    node->children[0] = left;
    node->children[1] = right;
    return node;
}


ASTNode* create_unary_operator_node(char *op) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = op;
    node->data.operator = strdup(op);
    node->numChildren = 1;
    node->children = (ASTNode**)malloc(sizeof(ASTNode*) * 1);
    return node;
}

ASTNode* create_function_call_node(char *id, ASTNode *args) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = FUNCTION_CALL;
    node->data.IdentifierData.identifier = strdup(id);
    node->numChildren = 1;
    node->children = (ASTNode**)malloc(sizeof(ASTNode));
    node->children[0] = args;
    return node;
}

ASTNode* create_constructor_call_node(char *id) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = CONSTRUCTOR_CALL;
    node->data.IdentifierData.identifier = init_identifier(strdup(id), NULL);
    return node;
}

ASTNode* create_variable_declaration_node(char *id, ASTNode *value) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = VARIABLE_DECL;
    node->data.IdentifierData.identifier = init_identifier(strdup(id), type_to_string(value->type));
    node->numChildren = 1;
    node->children = (ASTNode**)malloc(sizeof(ASTNode*) * 1);
    node->children[0] = value;
    return node;
}
ASTNode * create_variable_call_node(ASTNode * id)
{
    ASTNode * node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = VARIABLE_CALL;
    node->data.IdentifierData.identifier = init_identifier(strdup(id.data.IdentifierData.identifier), NULL)
    return node;
}

ASTNode* create_function_declaration_node(char *id, char *datatype, ASTNode *args, ASTNode *body) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = FUNCTION_DECL;
    node->data.IdentifierData = init_identifier(strdup(id), strdup(datatype));
    node->numChildren = 2;
    node->children = (ASTNode**)malloc(sizeof(ASTNode) * 2);
    node->children[0] = args;
    node->children[1] = body;
    return node;
}

ASTNode * create_class_declaration_node(char *id, ASTNode *body) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = CLASS_DECL;
    node->data.IdentifierData = init_identifier(strdup(id), NULL);
    node->numChildren = 1;
    node->children = (ASTNode**)malloc(sizeof(ASTNode));
    node->children[0] = body;
    return node;
}

ASTNode * create_constructor_declaration_node(ASTNode * args, ASTNode * body)
{
    ASTNode * node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = CONSTRUCTOR_DECL;
    node->numChildren = 2;
    node->children = (ASTNode**)malloc(sizeof(ASTNode*) * 2);
    node->children[0] = args;
    node->children[1] = body;
    return node;
}

ASTNode* append_node(ASTNode *parent, ASTNode *child) {
    parent->numChildren++;
    parent->children = (ASTNode**)realloc(parent->children, sizeof(ASTNode*) * parent->numChildren);
    parent->children[parent->numChildren - 1] = child;
    return parent;
}

ASTNode * create_if_node(ASTNode * condition, ASTNode * body)
{
    ASTNode * node = malloc(sizeof(ASTNode));
    node->type = IF_STMT;
    node->numChildren = 2;
    node->children = malloc(sizeof(ASTNode*) * 2);
    node->children[0] = condition;
    node->children[1] = body;
    return node;
}
ASTNode * create_else_if_node(ASTNode * condition, ASTNode *body)
{
    ASTNode * node = malloc(sizeof(ASTNode));

    node->type = ELSE_IF_STMT;
    node->numChildren = 2;
    node->children = malloc(sizeof(ASTNode*) * 2);
    node->children[0] = condition;
    node->children[1] = body;
    return node;
}

ASTNode * create_else_node(ASTNode * body)
{
    ASTNode * node = malloc(sizeof(ASTNode));

    node->type = ELSE_STMT;
    node->numChildren = 1;
    node->children = malloc(sizeof(ASTNode*));
    node->children[0] = body;
    return node;
}

ASTNode * create_while_loop_node(ASTNode * condition, ASTNode * body)
{
    ASTNode * node = malloc(sizeof(ASTNode));

    node->type = WHILE_LOOP_STMT;
    node->numChildren = 2;
    node->children = malloc(sizeof(ASTNode*) * 2);
    node->children[0] = condition;
    node->children[1] = body;
    return node;
}
ASTNode * create_for_loop_node(ASTNode * init, ASTNode * condition, ASTNode * update, ASTNode * body)
{
    ASTNode * node = malloc(sizeof(ASTNode));

    node->type = FOR_LOOP_STMT;
    node->numChildren = 4;
    node->children = malloc(sizeof(ASTNode*) * 4);
    
    node->children[0] = init;
    node->children[1] = condition;
    node->children[2] = update;
    node->children[3] = body;
    return node;
}

ASTNode * create_block_node(ASTNode ** statements, int numStatements)
{
    ASTNode * node = malloc(sizeof(ASTNode));

    node->type = BLOCK;
    node->numChildren = numStatements;
    node->children = malloc(sizeof(ASTNode*) * numStatements);
    for(int i = 0; i < numStatements; i++)
    {
        node->children[i] = statements[i];
    }
    return node;
}

char * type_to_string(NodeType type)
{
    switch(type)
    {
        case INT: return "int";
        case CHAR: return "char";
        case STRING: return "string";
        case BOOL: return "bool";
        case FLOAT: return "float";
        case VARIABLE_DECL: return "variable decl";
        case FUNCTION_DECL: return "function decl";
        default: return NULL;
    }
}