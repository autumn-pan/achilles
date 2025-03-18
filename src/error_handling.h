typedef struct ErrorList;
typedef struct Error;
typedef enum ERROR_TYPE;
Error *init_error(enum ERROR_TYPE type, char *message, int line, int column);
Error *init_syntax_error(char *expected, char *found, int line, int column);

void freeError(Error *error);

void print_errors(ErrorList *errorList);

void free_error_list(ErrorList *list);

ErrorList *create_error_list();

void add_error(ErrorList *list, Error *error);
