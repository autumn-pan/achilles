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
    OPERATOR
} NodeType;

// Define the types of statements
typedef enum {
    RETURN_STMT,
    IF_STMT,
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
        char *identifier;
        char *operator;
    } data;
} ASTNode;

ASTNode* createIntNode(int num) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = INT;
    node->data.intval = num;
    return node;
}

ASTNode* createFloatNode(int num) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = FLOAT;
    node->data.floatval = num;
    return node;
}

ASTNode* createCharNode(char c) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = CHAR;
    node->data.charval = c;
    return node;
}

ASTNode* createStringNode(int str) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = STRING;
    node->data.strval = strdup(str);
    return node;
}

ASTNode* createBoolNode(int boolean) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = BOOL;
    node->data.intval = boolean;
    return node;
}

ASTNode* createBinaryOperatorNode(char *op) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = OPERATOR;
    node->data.operator = strdup(op);
    node->numChildren = 2;
    node->children = (ASTNode**)malloc(sizeof(ASTNode*) * 2);
    return node;
}

ASTNode* createUnaryOperatorNode(char *op) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = OPERATOR;
    node->data.operator = strdup(op);
    node->numChildren = 1;
    node->children = (ASTNode**)malloc(sizeof(ASTNode*) * 1);
    return node;
}

ASTNode* createFunctionCallNode(char *id, ASTNode *args) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = FUNCTION_CALL;
    node->data.identifier = strdup(id);
    node->numChildren = 1;
    node->children = (ASTNode**)malloc(sizeof(ASTNode*));
    node->children[0] = args;
    return node;
}

ASTNode* createConstructorCallNode(char *id) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = CONSTRUCTOR_CALL;
    node->data.identifier = strdup(id);
    return node;
}

ASTNode* createVariableDeclarationNode(char *id, ASTNode *value) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = VARIABLE_DECL;
    node->data.identifier = strdup(id);
    node->numChildren = 1;
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

ASTNode* create_function_declaration_node(char *id, ASTNode *args, ASTNode *body) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = FUNCTION_DECL;
    node->data.identifier = strdup(id);
    node->numChildren = 2;
    node->children = (ASTNode**)malloc(sizeof(ASTNode*) * 2);
    node->children[0] = args;
    node->children[1] = body;
    return node;
}

ASTNode * create_class_declaration_node(char *id, ASTNode *body) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = CLASS_DECL;
    node->data.identifier = strdup(id);
    node->numChildren = 1;
    node->children = (ASTNode**)malloc(sizeof(ASTNode*));
    node->children[0] = body;
    return node;
}

ASTNode * create_constructor_declaration_node(char * id, ASTNode * args, ASTNode * body)
{
    ASTNode * node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = CONSTRUCTOR_DECL;
    node->data.identifier = strdup(id);
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