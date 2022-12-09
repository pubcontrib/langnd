#ifndef PARSE_H
#define PARSE_H

#include "utility.h"

typedef enum
{
    STATEMENT_TYPE_UNKNOWN,
    STATEMENT_TYPE_LITERAL,
    STATEMENT_TYPE_ASSIGNMENT,
    STATEMENT_TYPE_INVOKE,
    STATEMENT_TYPE_BRANCH,
    STATEMENT_TYPE_LOOP,
    STATEMENT_TYPE_CATCH,
    STATEMENT_TYPE_THROW,
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
    string_t *name;
} identifier_t;

typedef struct
{
    value_t *value;
} literal_statement_data_t;

typedef struct
{
    identifier_t *identifier;
    statement_t *value;
} assignment_statement_data_t;

typedef struct
{
    identifier_t *identifier;
    list_t *arguments;
} invoke_statement_data_t;

typedef struct
{
    statement_t *condition;
    list_t *body;
} conditional_branch_t;

typedef struct
{
    list_t *conditionals;
    list_t *fallback;
} branch_statement_data_t;

typedef struct
{
    statement_t *condition;
    list_t *body;
} loop_statement_data_t;

typedef struct
{
    list_t *body;
} catch_statement_data_t;

typedef struct
{
    statement_t *error;
} throw_statement_data_t;

typedef struct
{
    identifier_t *identifier;
} reference_statement_data_t;

typedef struct
{
    list_t *statements;
    string_t *errorMessage;
    string_t *hintMessage;
} script_t;

script_t *parse_script(string_t *code);
void destroy_script(script_t *script);
void destroy_statement(statement_t *statement);
void destroy_identifier(identifier_t *identifier);

#endif
