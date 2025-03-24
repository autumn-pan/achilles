#ifndef AST_H
#define AST_H


typedef enum NodeType {
    PROGRAM,
    STATEMENT,
    EXPRESSION,

    FOR_LOOP_STMT,
    WHILE_LOOP_STMT,
    BLOCK,
    IF_STMT,
    ELSE_IF_STMT,
    ELSE_STMT,

    FUNCTION_DECL,
    FUNCTION_CALL,

    CLASS_DECL,
    CONSTRUCTOR_DECL,
    CONSTRUCTOR_CALL,
    VARIABLE_DECL,
    VARIABLE_CALL,

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

typedef struct {
    char * identifier;
    char * datatype;
} IdentifierData;

typedef struct ASTNode {
    NodeType type;
    struct ASTNode **children;

    int numChildren;

    union {
        int intval;
        float floatval;
        char *strval;
        char charval;
        bool boolval;
        IdentifierData * IdentifierData;
        char *operator;
    } data;
} ASTNode;

char * type_to_string(NodeType type);
ASTNode * create_variable_declaration_node(char *identifier, ASTNode *value);
ASTNode * create_variable_call_node(ASTNode * id);
ASTNode * create_function_call_node(char *id, ASTNode *args);
ASTNode * create_function_declaration_node(char *id, char * datatype, ASTNode *args, ASTNode *body);
ASTNode * create_class_declaration_node(char *id, ASTNode *body);
ASTNode * create_constructor_declaration_node(ASTNode * args, ASTNode * body);
ASTNode * create_if_node(ASTNode * condition, ASTNode * body);
ASTNode * create_else_if_node(ASTNode * condition, ASTNode * body);
ASTNode * create_else_node(ASTNode * body);
ASTNode * create_while_loop_node(ASTNode * condition, ASTNode * body);
ASTNode * create_for_loop_node(ASTNode * init, ASTNode * condition, ASTNode * update, ASTNode * body);
ASTNode * create_block_node(ASTNode ** statements, int numStatements);
ASTNode * create_return_node(ASTNode * value);
ASTNode * create_int_node(int num);
ASTNode * create_string_node(char * str);
ASTNode * create_float_node(float num);
ASTNode * create_boolean_node(bool boolean);
ASTNode * create_binary_operator_node(NodeType op, ASTNode *left, ASTNode *right);
ASTNode * create_unary_operator_node(NodeType op, ASTNode * child);
ASTNode * create_constructor_call_node(char *id);

#endif