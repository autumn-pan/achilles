#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./tokens.c"


//This defines the structure of a token
typedef struct {
    enum TOKEN_TYPE type;
    char *value;
    int line;
    int column;
} Token;


//Constructor function that creates a token
Token initToken(enum TOKEN_TYPE type, char *value, int line, int column)
{
    Token token; // Initialize token struct
    token.type = type; // Set type
    token.value = strdup(value); // Set internal value
    token.line = line; // Line value for error handling
    token.column = column; // Col value for error handling
    return token; // Returns the constructed token
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
    if(c >= 48 && c <= 57)
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
Lexer * init_lexer(const char *src) {
    //Initialize the lexer
    Lexer *lexer = (Lexer *)malloc(sizeof(Lexer)); 
    // Set the source code
    lexer->src = src;
    //Set the length
    lexer->length = strlen(src);

    //Set the current position that the lexer is on
    lexer->pos = 0;

    //Set the line
    lexer->line = 1;

    //Set the column
    lexer->column = 1;

    //Return the finished lexer
    return lexer;
};


//Checks if a string is in an array
bool inArray(const char **arr, const char *key)
{
    // For every string of the array
    for(int i = 0; arr[i] != '\0'; i++) 
    {
        // Compare each value with the key
        if(strcmp((const char *)arr[i], key) == 0)
            return true; //If they are equal, return true
    }
    // If no string matches, return false
    return false;
}

bool charInArray(const char *arr, const char key)
{
    // For every char of the array
    for(int i = 0; arr[i] != '\0'; i++) 
    {
        // Compare each value with the key
        if((int)arr[i] == (int)key)
            return true; //If they are equal, return true
    }
    // If no string matches, return false
    return false;
}


//Returns current char, moves the lexer one character forward
char advance(Lexer *lexer)
{
    // Checks for EOF
    if(lexer->pos >= lexer->length)
        return '\0';
    

    char c = lexer->src[lexer->pos];

    // If it is not the EOF, move the position forward
    lexer->pos++;
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
    if(lexer->pos < lexer->length - 1)
        return lexer->src[lexer->pos + 1];
    //Otherwise, return EOF
    return '\0';
};



// Check if a char is whitespace or not
bool isWhitespace(Lexer *lexer)
{
    char c = lexer->src[lexer->pos];
    // Check if it is a space, tab, or newline
    if(c == ' ' || c == '\t' || c == '\n')
        return true;
    // If not, return false
    return false;

}

char * toString(const char character)
{
    static char str[2];
    str[0] = character;
    str[1] = '\0';
    return str;
}




// Analyze the next token
Token nextToken(Lexer *lexer)
{
    // This stores the Token type
    enum TOKEN_TYPE type;
    // This is where the values for tokens will be stored.
    char val[256] = "";
    // Line and Column
    int l = lexer->line;
    int c = lexer->column;


    //Check for EOF
    if(lexer->pos >= lexer->length)
        return initToken(END_OF_FILE, "", l, c);

    //Checks if it is a string
    if(lexer->src[lexer->pos] == '"')
    {
        //Excludes the first quotation mark
        advance(lexer);
        while (true) // Loops until broken
        {
            //If the quotation mark has a \ behind it. If so, count it as part of the string
            if (lookahead(lexer) == '"' && lexer->src[lexer->pos] == '\\')
            {
                strcat(val, toString(advance(lexer))); 

            } 
            else if (lexer->src[lexer->pos] == '"') // Terminates string
            {
                advance(lexer); // Excludes the last quotation mark
                break;
            }
            else // builds the str
            {
                strcat(val, toString(advance(lexer))); 
            }
        }
        return initToken(STR_LITERAL, val, l, c); // returns string literal
    }
    else if (alpha(lexer->src[lexer->pos]) || lexer->src[lexer->pos] == '_') // Checks if it's alphanumeric; will construct keywords and identifiers
    {
        while(alphanum(lexer->src[lexer->pos]) || lexer->src[lexer->pos] == '_')
        {
            strcat(val, toString(advance(lexer))); 
        }
        if(inArray(KEYWORDS, val))
            return initToken(KEYWORD, val, l, c);
        return initToken(IDENTIFIER, val, l, c);
    }
    else if(num(lexer->src[lexer->pos])) //Checks for a numeric literal
    {
        while(num(lexer->src[lexer->pos])) //Initiates the first component of the literal
        {
            strcat(val, toString(advance(lexer))); 
        }
        if(lexer->src[lexer->pos] == '.') //If a dot is found, then it is a float
        {
            type = FLOAT_LITERAL;

            advance(lexer); // Excludes the dot
            strcat(val, "."); // Adds the dot to the float

            while(num(lexer->src[lexer->pos]))
            {
                strcat(val, toString(advance(lexer))); //Construct the second part of the float
            } 
            return initToken(FLOAT_LITERAL, val, l, c); // Return float literal
        }
        else
            return initToken(INT_LITERAL, val, l, c); // Return int literal
    }

    if(charInArray(OPERATOR_CHARS, lexer->src[lexer->pos])) //Check if token is an operator
    {
        while(charInArray(OPERATOR_CHARS, lexer->src[lexer->pos]))
        {
            strcat(val, toString(advance(lexer)));
        }

        if(inArray(OPERATORS, val))
            return initToken(OPERATOR, val, l, c);
        return initToken(UNKNOWN, val, l, c);
    }

    //Check if token is a misc token
    switch(lexer->src[lexer->pos]) { //Check for misc tokens
        case '{': advance(lexer); return initToken(LBRACE, "{", l, c);
        case '}': advance(lexer); return initToken(RBRACE, "}", l, c);
        case '[': advance(lexer); return initToken(LSQR, "[", l, c);
        case ']': advance(lexer); return initToken(RSQR, "]", l, c);
        case '(': advance(lexer); return initToken(LPAR, "(", l, c);
        case ')': advance(lexer); return initToken(RPAR, ")", l, c);
        case '.': advance(lexer); return initToken(DOT, ".", l, c);
        case ';': advance(lexer); return initToken(SEMI, ";", l, c);
        case ',': advance(lexer); return initToken(COMMA, ",", l, c);
        case ':': advance(lexer); return initToken(COLON, ":", l, c);
        case '\n': advance(lexer); return initToken(EOL, "\n", l, c);

       //If not, then it is unknown 
        default: {
            strcat(val, toString(advance(lexer)));
            return initToken(UNKNOWN, val, l, c);
        }
    }
}


//For testing purposes
int main() {
    Lexer *lexer = init_lexer("int:num = 5");
    Token token;

    while (token.type != END_OF_FILE)  // While the lexer is not at the end of the file 
    {
        // Skip whitespace
        while (isWhitespace(lexer))
            advance(lexer);

        // Take the next token
        token = nextToken(lexer);

        // Print the token
        printf("Type: %s, Value: %s \n", getTokenType(token.type), token.value);

        // Free the allocated memory
        free(token.value);
    }

    free(lexer);
    return 0;
}