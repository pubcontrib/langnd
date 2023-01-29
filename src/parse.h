#ifndef PARSE_H
#define PARSE_H

#include "utility.h"

typedef enum
{
    EXPRESSION_TYPE_UNKNOWN,
    EXPRESSION_TYPE_LITERAL,
    EXPRESSION_TYPE_REFERENCE,
    EXPRESSION_TYPE_ASSIGNMENT,
    EXPRESSION_TYPE_INVOKE,
    EXPRESSION_TYPE_BRANCH,
    EXPRESSION_TYPE_LOOP,
    EXPRESSION_TYPE_CATCH,
    EXPRESSION_TYPE_BREAK,
    EXPRESSION_TYPE_CONTINUE,
    EXPRESSION_TYPE_THROW,
    EXPRESSION_TYPE_SNIPPET
} expression_type_t;

typedef struct
{
    expression_type_t type;
    void *data;
} expression_t;

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
} literal_expression_data_t;

typedef struct
{
    identifier_t *identifier;
} reference_expression_data_t;

typedef struct
{
    identifier_t *identifier;
    expression_t *value;
} assignment_expression_data_t;

typedef struct
{
    identifier_t *identifier;
    list_t *arguments;
} invoke_expression_data_t;

typedef struct
{
    expression_t *condition;
    expression_t *action;
} conditional_branch_t;

typedef struct
{
    list_t *branches;
} branch_expression_data_t;

typedef struct
{
    expression_t *condition;
    expression_t *action;
} loop_expression_data_t;

typedef struct
{
    expression_t *action;
} catch_expression_data_t;

typedef struct
{
    expression_t *pick;
} break_expression_data_t;

typedef struct
{
    expression_t *pick;
} continue_expression_data_t;

typedef struct
{
    expression_t *error;
} throw_expression_data_t;

typedef struct
{
   list_t *expressions;
} snippet_expression_data_t;

typedef struct
{
    list_t *expressions;
    string_t *errorMessage;
    string_t *hintMessage;
} script_t;

script_t *parse_script(string_t *code);
void destroy_script(script_t *script);
void destroy_expression(expression_t *expression);
void destroy_identifier(identifier_t *identifier);

#endif
