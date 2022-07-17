#ifndef PARSE_H
#define PARSE_H

#include <stddef.h>
#include "lex.h"

typedef enum
{
    STATEMENT_TYPE_UNKNOWN,
    STATEMENT_TYPE_NUMBER,
    STATEMENT_TYPE_ASSIGNMENT,
    STATEMENT_TYPE_INVOKE,
    STATEMENT_TYPE_REFERENCE
} statement_type_t;

typedef struct
{
    statement_type_t type;
    void *data;
} statement_t;

typedef enum
{
    IDENTIFIER_TYPE_VARIABLE,
    IDENTIFIER_TYPE_FUNCTION
} identifier_type_t;

typedef struct
{
    identifier_type_t type;
    char *name;
} identifier_t;

typedef struct argument_link_t
{
    statement_t *argument;
    struct argument_link_t *next;
} argument_link_t;

typedef struct
{
    double value;
} number_statement_data_t;

typedef struct
{
    identifier_t *identifier;
    statement_t *value;
} assignment_statement_data_t;

typedef struct
{
    identifier_t *identifier;
    argument_link_t *arguments;
} invoke_statement_data_t;

typedef struct
{
    identifier_t *identifier;
} reference_statement_data_t;

typedef struct statement_link_t
{
    statement_t *statement;
    struct statement_link_t *next;
} statement_link_t;

typedef struct
{
    statement_link_t *statements;
    char *errorMessage;
    char *hintMessage;
} script_t;

script_t *parse_script(char *code);
void destroy_script(script_t *script);
void destroy_statement(statement_t *statement);
void destroy_identifier(identifier_t *identifier);

#endif
