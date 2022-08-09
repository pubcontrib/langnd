#ifndef LEX_H
#define LEX_H

#include <stddef.h>

typedef enum
{
    TOKEN_TYPE_UNKNOWN,
    TOKEN_TYPE_WHITESPACE,
    TOKEN_TYPE_COMMENT,
    TOKEN_TYPE_NUMBER,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_IDENTIFIER,
    TOKEN_TYPE_SYMBOL
} token_type_t;

typedef struct
{
    token_type_t type;
    size_t start;
    size_t end;
} token_t;

typedef enum
{
    SCANNER_STATE_RUNNING,
    SCANNER_STATE_ERRORED,
    SCANNER_STATE_COMPLETED
} scanner_state_t;

typedef struct
{
    char *code;
    size_t codeLength;
    scanner_state_t state;
    token_t token;
} scanner_t;

void start_scanner(scanner_t *scanner, char *code);
void progress_scanner(scanner_t *scanner);

#endif
