#include <stdio.h>
#include <stdlib.h>
#include "../lexer/lexer.c"
#include "../lexer/tokens.c"

// Define the types of AST nodes
typedef enum {
    PROGRAM,
    STATEMENT,
    EXPRESSION,
    INT,
    FLOAT,
    STRING,
    BOOL,
    CHAR,
    ADD,
    SUB,
    MULT,
    DIV,
    MOD,

    ASSIGN,
    PLUS_EQUAL,
    MINUS_EQUAL,

    NOT,
    AND,
    OR,

    LESS,
    GREATER,
    LESS_EQUAL,
    GREATER_EQUAL,
    EQUAL,

    BIT_AND,
    BIT_OR,
    BIT_XOR,
    BIT_NOT,
    LEFT_SHIFT,
    RIGHT_SHIFT,

    INSTANCEOF,

    CONCATENATE
} NodeType;

// Define the types of statements
typedef enum {
    RETURN_STMT,
    IF_STMT,
    ELSE_IF_STMT,
    ELSE_STMT,
    WHILE_STMT,
    FOR_STMT,
    BREAK_STMT,
    CONTINUE_STMT,

    FUNCTION_DECL,
    CONSTRUCTOR_DECL,
    CLASS_DECL,
    VARIABLE_DECL,

    ASSIGNMENT_STMT,
    EXPRESSION_STMT,
    BLOCK_STMT,

    FUNCTION_CALL,
    CONSTRUCTOR_CALL,
    VARIABLE_CALL,
} StatementType;

// Define the structure of an AST node
typedef struct ASTNode {
    NodeType type;
    struct ASTNode **children;
    int numChildren;

    union {
        int intval;
        float floatval;
        char *strval;
        char charval;
        typedef struct {
            char * identifier;
            char * datatype;
        } IdentifierData;
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

ASTNode* create_string_node(int str) {
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
    node->type = OPERATOR;
    node->data.operator = strdup(op);
    node->numChildren = 1;
    node->children = (ASTNode**)malloc(sizeof(ASTNode*) * 1);
    return node;
}

ASTNode* create_function_call_node(char *id, ASTNode *args) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = FUNCTION_CALL;
    node->data.identifier = strdup(id);
    node->numChildren = 1;
    node->children = (ASTNode**)malloc(sizeof(ASTNode*));
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
    node->value = value;
    node->children = (ASTNode**)malloc(sizeof(ASTNode*) * 1);
    switch(value->type) {
        case INT_LITERAL:
            node->children[0] = createIntNode(value->data.intval);
            break;
        case FLOAT_LITERAL:
            node->children[0] = createFloatNode(value->data.floatval);
            break;
        case CHAR_LITERAL:
            node->children[0] = createCharNode(value->data.charval);
            break;
        case STR_LITERAL:
            node->children[0] = createStringNode(value->data.strval);
            break;
        case BOOL_LITERAL:
            node->children[0] = createBoolNode(value->data.intval);
            break;
    }
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
    node->data.IdentifierData.identifier = init_identifier(strdup(id), NULL)
    node->numChildren = 2;
    node->children = (ASTNode**)malloc(sizeof(ASTNode*) * 2);
    node->children[0] = args;
    node->children[1] = body;
    return node;
}

ASTNode * create_class_declaration_node(char *id, ASTNode *body) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = CLASS_DECL;
    node->data.IdentifierData.identifier = init_identifier(strdup(id), NULL)
    node->numChildren = 1;
    node->children = (ASTNode**)malloc(sizeof(ASTNode*));
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

    node->type = WHILE_STMT;
    node->numChildren = 2;
    node->children = malloc(sizeof(ASTNode*) * 2);
    node->children[0] = condition;
    node->children[1] = body;
    return node;
}
ASTNode * create_for_loop_node(ASTNode * init, ASTNode * condition, ASTNode * update, ASTNode * body)
{
    ASTNode * node = malloc(sizeof(ASTNode));

    node->type = FOR_STMT;
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

    node->type = BLOCK_STMT;
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