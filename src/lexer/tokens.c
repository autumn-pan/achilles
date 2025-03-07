#include <stdbool.h>
//This file contains the tokens for the lexer, as well as keywords, operators, and grouper tokens.
typedef enum TOKEN_TYPE {
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
};

const char *KEYWORDS = {
    "function",
    "return",

    "if",
    "else",

    "class",
    "constructor",

    "static",
    
    "const",
    "mutable"

    "public",
    "private",

    "while",
    "for",

    "break",
    "continue",

    "extends",
    
    "float",
    "int",
    "string"
    "char",
    "bool",
    "void",

    "in",
    "instanceof",

    "new",

    "enum"
};

const char *OPERATORS = {//List of each operator
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

const char OPERATOR_CHARS = {//Every operator character
    '+',
    '-',
    '*',
    '/',
    '>',
    '<',
    '='
};

const char *GROUPER_TOKENS = {//List of grouper values
    "{",
    "}",
    "[",
    "]",
    "(",
    ")",
    "."
};

