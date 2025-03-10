#include <stdio.h>
#include <stdlib.h>


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
    IDENTIFIER,
    OPERATOR,
    FUNCTION_CALL,
    CONSTRUCTOR_CALL
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

ASTNode* createIdentifierNode(char *id) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = IDENTIFIER;
    node->data.identifier = strdup(id);
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

ASTNode* createFunctionCallNode(char *id) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = FUNCTION_CALL;
    node->data.identifier = strdup(id);
    return node;
}

ASTNode* createConstructorCallNode(char *id) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = CONSTRUCTOR_CALL;
    node->data.identifier = strdup(id);
    return node;
}
