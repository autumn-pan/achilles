typedef enum ERROR_TYPE {
    SYNTAX_ERROR,
    TYPE_ERROR
};

typedef struct {
    enum ERROR_TYPE type;
    char *message;
    int line;
    int column;
} Error;

typedef struct {
    Error **errors;
    int numErrors;
} ErrorList;

Error *init_error(enum ERROR_TYPE type, char *message, int line, int column) {
    Error *error = malloc(sizeof(Error));
    error->type = type;
    error->message = strdup(message);
    error->line = line;
    error->column = column;
    return error;
}

Error *init_syntax_error(char *expected, char *found, int line, int column) {
    char *message = malloc(strlen(expected) + strlen(found) + 100);
    sprintf(message, "Syntax Error: Expected %s, but found %s", expected, found);
    return init_error(SYNTAX_ERROR, message, line, column);
}

void freeError(Error *error) {
    free(error->message);
    free(error);
}


void print_errors(ErrorList *errorList) {
    for (int i = 0; i < errorList->numErrors; i++) {
        Error *error = errorList->errors[i];
        printf("Error: %s at line %d, column %d\n", error->message, error->line, error->column);
    }
    printf("Total errors: %d\n", errorList->numErrors);
}
