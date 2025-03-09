#include <stdbool.h>
//This file contains the tokens for the lexer, as well as keywords, operators, and grouper tokens.
typedef enum {
    KEYWORD,
    IDENTIFIER,
    STR_LITERAL,
    FLOAT_LITERAL,
    INT_LITERAL,
    CHAR_LITERAL,
    BOOL_LITERAL,
    OPERATOR,
    LBRACE,
    RBRACE,
    LSQR,
    RSQR,
    LPAR,
    RPAR,
    UNKNOWN,
    EOL,
    DOT
} TOKEN_TYPE;

// Corrected the syntax for the array of keywords
const char *KEYWORDS[] = {
    "function",
    "return",
    "if",
    "else",
    "class",
    "constructor",
    "static",
    "const",
    "mutable",
    "public",
    "private",
    "while",
    "for",
    "break",
    "continue",
    "extends",
    "float",
    "int",
    "string",
    "char",
    "bool",
    "void",
    "in",
    "instanceof",
    "new",
    "enum"
};

// Corrected the syntax for the array of operators
const char *OPERATORS[] = {//List of each operator
    "+",
    "-",
    "*",
    "/",
    "++",
    "--",
    "**",
    ">>",
    "<<",
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

const char OPERATOR_CHARS[] = {//Every operator character
    '+',
    '-',
    '*',
    '/',
    '>',
    '<',
    '='
};

// Corrected the syntax for the array of grouper tokens
const char GROUPER_TOKENS[7] = {//List of grouper values
    '{',
    '}',
    '[',
    ']',
    '(',
    ')',
    '.'
};

