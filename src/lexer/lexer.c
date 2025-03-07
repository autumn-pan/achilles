#include <stdbool.h>
#include "./tokens.c"
//This defines the structure of a token
typedef struct {
    char *type;
    char *value;
    int line;
    int column;
} Token;
//Constructor function that creates a token
Token initToken(enum TOKEN_TYPE type, char *value, int line, int column)
{
    Token *token = (Token *)malloc(sizeof(Token)); // Initialize token struct
    token->type = type; // Set type
    token->value = strdup(value); // Set internal value
    token->line = line; // Line value for error handling
    token->column = column; // Col value for error handling
    return *token; // Returns the constructed token
}

//Check if a character is alphanumeric
bool alphanum(char c)
{
    if((c >= 65 && c <=90) || (c >= 48 && c <= 57) || (c >= 97 && c <= 122)) // Checks if it is a capital letter, digit, or lowercase letter
        return true; 
    return false;
}

// Check if a character is alphabetical
bool alpha(char c)
{
    if ((c >= 65 && c <=90) || (c >= 97 && c <=122)) // Checks for lowercase, then uppercase
        return true;
    // If it is neither, return false
    return false;
}

// Check if a character is a digit
bool num(char c)
{
    // Checks if its ASCII value matchs that of a digit
    if(c >= 48 && c <= 58)
        return true;

    //If not, return false
    return false;
}



// Structure for a lexer
typedef struct {
    const char *src;  // Pointer to source code
    int length;          // Length of source code
    int pos;        // Current index in source
    int line;            // Line number for error reporting
    int column;          // Column number for debugging
} Lexer;

// Initialize the lexer
Lexer init_lexer(const char *src) {
    //Initialize the lexer
    Lexer lexer; 
    // Set the source code
    lexer.src = src;
    //Set the length
    lexer.length = strlen(src);

    //Set the current position that the lexer is on
    lexer.pos = 0;

    //Set the line
    lexer.line = 1;

    //Set the column
    lexer.column = 1;

    //Return the finished lexer
    return lexer;
};


//Checks if a string is in an array
bool inArray(char *arr, char *key)
{
    // For every string of the array
    for(int i = 0; i < sizeof(arr); i++) 
    {
        // Compare each value with the key
        if(strcmp(arr[i],key))
            return true; //If they are equal, return true
    }
    // If no string matches, return false
    return false;
}

//Move the lexer one character forward
char advance(Lexer *lexer)
{
    // Checks for EOF
    if(lexer->pos >= lexer->length)
        return '\0';
    
    // If it is not the EOF, move the position forward
    lexer->pos++;

    //Handle new lines

    char c = lexer->src[lexer->pos];
    // If current char is EOL
    if(c == '\n')
    {
        // Increment line, reset column
        lexer->line++;
        lexer->column = 1;
    }
    return c;
};


// Check the next character without moving the lexer
char lookahead(Lexer *lexer) 
{
    // If it is not the EOF, return the next value
    if(lexer->pos < lexer->length)
        return lexer->src[lexer->pos + 1];
    //Otherwise, return EOF
    return '\0';
};


// Analyze the next token
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

    //Check if token is a misc token
    switch(lookahead(lexer)) { //Check for misc tokens
        case '\{': return initToken(LBRACE, '{', l, c);
        case '\}': return initToken(RBRACE, '\}', l, c);
        case '\[': return initToken(LSQR, '\[', l, c);
        case '\]': return initToken(RSQR, '\]', l, c);
        case '\(': return initToken(LPAR, '\(', l, c);
        case '\)': return initToken(RPAR, '\)', l, c);
        case '\.': return initToken(DOT, '\.', l, c);
       //If not, then it is unknown 
        default: return initToken(UNKNOWN, lexer->src[lexer->pos], l, c);
    }
    
}
