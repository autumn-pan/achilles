typedef struct {
    enum TOKEN_TYPE type;
    char *value;
    int line;
    int column;
} Token;

typedef struct {
    Token * token_stream;
} TokenStream;
                                                                                                                                                    
typedef struct {
    const char *src;  // Pointer to source code
    int length;          // Length of source code
    int pos;        // Current index in source
    int line;            // Line number for error reporting
    int column;          // Column number for debugging
} Lexer;

struct parser;
bool alphanum(char c);
bool alpha(char c);
bool num(char c);
bool inArray(const char *array[], const char *value);
bool charInArray(const char *array, char value);
char * toString(const char character);
char advance(Lexer *lexer);
char lookahead(Lexer *lexer);
Lexer *init_lexer(const char *src);
Token initToken(enum TOKEN_TYPE type, char *value, int line, int column);
Token nextToken(Lexer *lexer);
bool isWhitespace(Lexer *lexer);
TokenStream * tokenize();
