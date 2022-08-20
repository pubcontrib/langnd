#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "execute.h"
#include "parse.h"
#include "utility.h"

typedef enum
{
    VALUE_TYPE_NULL,
    VALUE_TYPE_NUMBER,
    VALUE_TYPE_STRING
} value_type_t;

typedef struct
{
    value_type_t type;
    void *data;
    int thrown;
    int owned;
} value_t;

static value_t *apply_statement(statement_t *statement, map_t *variables);
static value_t *throw_error(char *message);
static value_t *new_null();
static value_t *new_number(double number);
static value_t *new_string(char *string);
static void destroy_value(value_t *value);
static void destroy_value_unsafe(void *value);

outcome_t *execute(char *code)
{
    outcome_t *outcome;
    script_t *script;
    statement_link_t *link;
    map_t *variables;

    outcome = allocate(sizeof(outcome_t));
    outcome->errorMessage = NULL;
    outcome->hintMessage = NULL;
    script = parse_script(code);

    if (script->errorMessage)
    {
        outcome->errorMessage = script->errorMessage;
        outcome->hintMessage = script->hintMessage;
        script->errorMessage = NULL;
        script->hintMessage = NULL;
        destroy_script(script);
        return outcome;
    }

    variables = empty_map(hash_string, destroy_value_unsafe, 8);
    link = script->statements;

    while (link)
    {
        value_t *result;

        result = apply_statement(link->statement, variables);

        if (result->thrown)
        {
            if (result->type == VALUE_TYPE_STRING)
            {
                char *message;

                message = copy_string(result->data);

                if (!result->owned)
                {
                    destroy_value(result);
                }

                outcome->errorMessage = copy_string("failed to execute code");
                outcome->hintMessage = message;
            }
            else
            {
                crash_with_message("unsupported branch EXECUTE_THROWN_MESSAGE");
            }

            break;
        }

        if (!result->owned)
        {
            destroy_value(result);
        }

        link = link->next;
    }

    destroy_map(variables);
    destroy_script(script);

    return outcome;
}

void destroy_outcome(outcome_t *outcome)
{
    if (outcome->errorMessage)
    {
        free(outcome->errorMessage);
    }

    if (outcome->hintMessage)
    {
        free(outcome->hintMessage);
    }

    free(outcome);
}

static value_t *apply_statement(statement_t *statement, map_t *variables)
{
    switch (statement->type)
    {
        case STATEMENT_TYPE_NUMBER:
        {
            number_statement_data_t *data;

            data = statement->data;

            return new_number(data->value);
        }

        case STATEMENT_TYPE_STRING:
        {
            string_statement_data_t *data;

            data = statement->data;

            return new_string(data->value);
        }

        case STATEMENT_TYPE_ASSIGNMENT:
        {
            assignment_statement_data_t *data;
            value_t *value;

            data = statement->data;
            value = apply_statement(data->value, variables);

            if (value->thrown)
            {
                return value;
            }

            value->owned = 1;
            set_map_item(variables, copy_string(data->identifier->name), value);

            return new_null();
        }

        case STATEMENT_TYPE_INVOKE:
        {
            invoke_statement_data_t *data;

            data = statement->data;

            if (strcmp(data->identifier->name, "add") == 0)
            {
                argument_link_t *link;
                value_t *left, *right, *result;

                link = data->arguments;

                if (!link)
                {
                    return throw_error("absent argument");
                }

                left = apply_statement(link->argument, variables);

                if (left->thrown)
                {
                    return left;
                }

                link = link->next;

                if (!link)
                {
                    return throw_error("absent argument");
                }

                right = apply_statement(link->argument, variables);

                if (right->thrown)
                {
                    return right;
                }

                if (left->type == VALUE_TYPE_NUMBER)
                {
                    double *x, *y;

                    if (right->type != VALUE_TYPE_NUMBER)
                    {
                        return throw_error("wrong argument type");
                    }

                    x = (double *) left->data;
                    y = (double *) right->data;
                    result = new_number(x[0] + y[0]);
                }
                else if (left->type == VALUE_TYPE_STRING)
                {
                    char *x, *y;
                    char *sum;
                    size_t length;

                    if (right->type != VALUE_TYPE_STRING)
                    {
                        return throw_error("wrong argument type");
                    }

                    x = (char *) left->data;
                    y = (char *) right->data;
                    length = strlen(x) + strlen(y);
                    sum = allocate(sizeof(char) * length + 1);
                    memcpy(sum, x, strlen(x));
                    memcpy(sum + strlen(x), y, strlen(y));
                    result = new_string(sum);
                    free(sum);
                }
                else
                {
                    return throw_error("wrong argument type");
                }

                if (!left)
                {
                    destroy_value(left);
                }

                if (!right)
                {
                    destroy_value(right);
                }

                return result;
            }
            else if (strcmp(data->identifier->name, "write") == 0)
            {
                argument_link_t *link;
                value_t *message, *file;
                char *text;

                link = data->arguments;

                if (!link)
                {
                    return throw_error("absent argument");
                }

                message = apply_statement(link->argument, variables);

                if (message->thrown)
                {
                    return message;
                }

                link = link->next;

                if (!link)
                {
                    return throw_error("absent argument");
                }

                file = apply_statement(link->argument, variables);

                if (file->thrown)
                {
                    return file;
                }

                if (message->type != VALUE_TYPE_STRING)
                {
                    return throw_error("wrong argument type");
                }

                text = (char *) message->data;

                if (file->type == VALUE_TYPE_NUMBER)
                {
                    FILE *streamHandle;
                    double streamID;

                    streamID = ((double *) file->data)[0];

                    if (streamID == 1.0)
                    {
                        streamHandle = stdout;
                    }
                    else if (streamID == 2.0)
                    {
                        streamHandle = stderr;
                    }
                    else
                    {
                        return throw_error("absent file");
                    }

                    fprintf(streamHandle, "%s", text);
                    fflush(streamHandle);

                    return new_null();
                }
                else if (file->type == VALUE_TYPE_STRING)
                {
                    FILE *fileHandle;
                    char *fileName;

                    fileName = ((char *) file->data);
                    fileHandle = fopen(fileName, "wb");

                    if (fileHandle)
                    {
                        fwrite(text, sizeof(char), strlen(text), fileHandle);

                        if (ferror(fileHandle))
                        {
                            fclose(fileHandle);

                            return throw_error("unable to write to file");
                        }

                        return new_null();
                    }
                    else
                    {
                        return throw_error("absent file");
                    }

                    return new_null();
                }
                else
                {
                    return throw_error("wrong argument type");
                }
            }
            else if (strcmp(data->identifier->name, "stringify") == 0)
            {
                argument_link_t *link;
                value_t *value;
                char *string;

                link = data->arguments;

                if (!link)
                {
                    return throw_error("absent argument");
                }

                value = apply_statement(link->argument, variables);

                if (value->type == VALUE_TYPE_NUMBER)
                {
                    double number;
                    int limit, effect;

                    number = ((double *) value->data)[0];
                    limit = 50;
                    string = allocate(sizeof(char) * limit);
                    effect = sprintf(string, "%g", number);

                    if (effect < 0 || effect > limit)
                    {
                        crash_with_message("number to string overflow");
                    }

                    return new_string(string);
                }
                else if (value->type == VALUE_TYPE_STRING)
                {
                    return value;
                }
                else
                {
                    return throw_error("wrong argument type");
                }
            }

            return throw_error("absent function");
        }

        case STATEMENT_TYPE_REFERENCE:
        {
            reference_statement_data_t *data;

            data = statement->data;

            if (data->identifier->type == IDENTIFIER_TYPE_VARIABLE)
            {
                value_t *value;

                value = get_map_item(variables, data->identifier->name);

                if (value)
                {
                    return value;
                }
                else
                {
                    return throw_error("absent variable");
                }
            }
            else if (data->identifier->type == IDENTIFIER_TYPE_FUNCTION)
            {
                return new_null();
            }
        }

        default:
            break;
    }

    return new_null();
}

static value_t *throw_error(char *message)
{
    value_t *value;

    value = new_string(message);
    value->thrown = 1;

    return value;
}

static value_t *new_null()
{
    value_t *value;

    value = allocate(sizeof(value_t));
    value->type = VALUE_TYPE_NULL;
    value->data = NULL;
    value->thrown = 0;
    value->owned = 0;

    return value;
}

static value_t *new_number(double number)
{
    value_t *value;
    double *data;

    data = allocate(sizeof(double));
    data[0] = number;
    value = allocate(sizeof(value_t));
    value->type = VALUE_TYPE_NUMBER;
    value->data = data;
    value->thrown = 0;
    value->owned = 0;

    return value;
}

static value_t *new_string(char *string)
{
    value_t *value;
    char *data;

    data = copy_string(string);
    value = allocate(sizeof(value_t));
    value->type = VALUE_TYPE_STRING;
    value->data = data;
    value->thrown = 0;
    value->owned = 0;

    return value;
}

static void destroy_value(value_t *value)
{
    if (value->data)
    {
        free(value->data);
    }

    free(value);
}

static void destroy_value_unsafe(void *value)
{
    destroy_value((value_t *) value);
}
