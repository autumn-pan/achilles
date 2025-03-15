#include <stdbool.h>
//This file contains the tokens for the lexer, as well as keywords, operators, and grouper tokens.
typedef enum TOKEN_TYPE{
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
    DOT,
    END_OF_FILE,
    COMMA,
    SEMI,
    COLON
};

// List of Keywords
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

// List of Operators
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

const char GROUPER_TOKENS[] = {//List of grouper values
    '{',
    '}',
    '[',
    ']',
    '(',
    ')',
    '.',
    ';',
    ',',
    ':'
};

//Return token type as a string
const char* getTokenType(enum TOKEN_TYPE type) {
    switch (type) {
        case END_OF_FILE: return "END_OF_FILE";
        case STR_LITERAL: return "STR_LITERAL";
        case KEYWORD: return "KEYWORD";
        case IDENTIFIER: return "IDENTIFIER";
        case FLOAT_LITERAL: return "FLOAT_LITERAL";
        case INT_LITERAL: return "INT_LITERAL";
        case OPERATOR: return "OPERATOR";
        case LBRACE: return "LBRACE";
        case RBRACE: return "RBRACE";
        case LSQR: return "LSQR";
        case RSQR: return "RSQR";
        case LPAR: return "LPAR";
        case RPAR: return "RPAR";
        case DOT: return "DOT";
        case COLON: return "COLON";
        case UNKNOWN: return "UNKNOWN";

        default: return "UNKNOWN";
    }
}