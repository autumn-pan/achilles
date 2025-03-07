#include <stdbool.h>
#include "./tokens.c"

typedef struct {
    char *type;
    char *value;
    int line;
    int column;
} Token;

Token initToken(enum TOKEN_TYPE type, char *value, int line, int column)
{
    Token *token = (Token *)malloc(sizeof(Token));
    token->type = type;
    token->value = strdup(value);
    token->line = line;
    token->column = column;
    return *token;
}
bool alphanum(char c)
{
    if((c >= 65 && c <=90) || (c >= 48 && c <= 57) || (c >= 97 && c <= 122))
        return true;
    return false;
}
bool alpha(char c)
{
    if ((c >= 65 && c <=90) || (c >= 97 && c <=122))
        return true;
    return false;
}
bool num(char c)
{
    if(c >= 48 && c <= 58)
        return true;
    return false;
}
bool digit(char c)
{
    if (c >= 48 && c <= 57)
        return true;
    return false;
}

typedef struct {
    const char *src;  // Pointer to source code
    int length;          // Length of source code
    int pos;        // Current index in source
    int line;            // Line number for error reporting
    int column;          // Column number for debugging
} Lexer;

// Init lexer struct
Lexer init_lexer(const char *src) {
    Lexer lexer;
    lexer.src = src;
    lexer.length = strlen(src);

    lexer.pos = 0;
    lexer.line = 1;
    lexer.column = 1;
    return lexer;
};

bool inArray(char *arr, char *key)
{
    for(int i = 0; i < sizeof(arr); i++) 
    {
        if(strcmp(arr[i],key))
            return true;
    }
    return false;
}

char advance(Lexer *lexer)
{
    if(lexer->pos >= lexer->length)
        return '\0';
    lexer->pos++;
    char c = lexer->src[lexer->pos];
    if(c == '\n')
    {
        lexer->line++;
        lexer->column = 1;
    }
    return c;
};

char lookahead(Lexer *lexer) 
{
    if(lexer->pos < lexer->length)
        return lexer->src[lexer->pos + 1];
    return '\0';
};

Token nextToken(Lexer *lexer)
{
    // This stores the Token type
    enum TOKEN_TYPE type;
    // This is where the values for tokens will be stored.
    char *val = "";
    // Line and Column
    int l = lexer->line;
    int c = lexer->column;
    //Checks if it is a string
    if(lookahead(lexer) == '"')
    {
        //Excludes the first quotation mark
        advance(lexer);
        while (true) // Loops until broken
        {
            //If the quotation mark has a \ behind it. If so, count it as part of the string
            if (peek(lexer) == '"' && lexer->src[lexer->pos] == '\\')
            {
                val += advance(lexer);

            } 
            else if (peek(lexer) == '"') // Terminates string
            {
                advance(lexer);
                break;
            }
            else // builds the str
            {
                val += advance(lexer);
            }
        }
        return initToken(STR_LITERAL, val, l, c); // returns string literal
    }
    else if (alpha(lookahead(lexer)) || lexer->src[lexer->pos] == '_') // Checks if it's alphanumeric; will construct keywords and identifiers
    {
        while(alphanum(lexer->src[lexer->pos]) || lexer->src[lexer->pos] == '_')
        {
            val += advance(lexer);
        }
        if(inArray(KEYWORDS, val))
            return initToken(KEYWORD, val, l, c);
        return initToken(IDENTIFIER, val, l, c);
    }
    else if(num(lookahead(lexer))) //Checks for a numeric literal
    {
        while(num(lookahead(lexer))) //Initiates the first component of the literal
        {
            val += advance(lexer);
        }
        if(lookahead(lexer) =='.') //If a dot is found, then it is a float
        {
            type = FLOAT_LITERAL;
            while(num(lookahead(lexer)))
            {
                val += advance(lexer); //Construct the second part of the float
            } 
            return initToken(FLOAT_LITERAL, val, l, c); // Return float literal
        }
        else
            return initToken(INT_LITERAL, val, l, c); // Return int literal
    }


    switch(lookahead(lexer)) { //Check for misc tokens
        case '{': return initToken(LBRACE, '{', l, c);
        case '}': return initToken(RBRACE, '}', l, c);
        case '[': return initToken(LSQR, '[', l, c);
        case ']': return initToken(RSQR, ']', l, c);
        case '(': return initToken(LPAR, '(', l, c);
        case ')': return initToken(RPAR, ')', l, c);
        case '.': return initToken(DOT, '.', l, c);
        default: return initToken(UNKNOWN, lexer->src[lexer->pos], l, c);
    }
    
}
