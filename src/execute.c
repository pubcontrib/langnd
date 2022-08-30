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
static value_t *new_number(number_t number);
static value_t *new_string(char *string);
static void destroy_value(value_t *value);
static void destroy_value_unsafe(void *value);

outcome_t *execute(char *code)
{
    outcome_t *outcome;
    script_t *script;
    list_node_t *node;
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
    node = script->statements->head;

    while (node)
    {
        value_t *result;

        result = apply_statement(node->value, variables);

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

        node = node->next;
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
                list_node_t *node;
                value_t *left, *right, *result;

                node = data->arguments->head;

                if (!node)
                {
                    return throw_error("absent argument");
                }

                left = apply_statement(node->value, variables);

                if (left->thrown)
                {
                    return left;
                }

                node = node->next;

                if (!node)
                {
                    return throw_error("absent argument");
                }

                right = apply_statement(node->value, variables);

                if (right->thrown)
                {
                    return right;
                }

                if (left->type == VALUE_TYPE_NUMBER)
                {
                    number_t *x, *y, sum;

                    if (right->type != VALUE_TYPE_NUMBER)
                    {
                        return throw_error("wrong argument type");
                    }

                    x = (number_t *) left->data;
                    y = (number_t *) right->data;

                    if (add_numbers(x[0], y[0], &sum) != 0)
                    {
                        return throw_error("arithmetic error");
                    }

                    result = new_number(sum);
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
            else if (strcmp(data->identifier->name, "subtract") == 0)
            {
                list_node_t *node;
                value_t *left, *right, *result;

                node = data->arguments->head;

                if (!node)
                {
                    return throw_error("absent argument");
                }

                left = apply_statement(node->value, variables);

                if (left->thrown)
                {
                    return left;
                }

                node = node->next;

                if (!node)
                {
                    return throw_error("absent argument");
                }

                right = apply_statement(node->value, variables);

                if (right->thrown)
                {
                    return right;
                }

                if (left->type == VALUE_TYPE_NUMBER)
                {
                    number_t *x, *y, difference;

                    if (right->type != VALUE_TYPE_NUMBER)
                    {
                        return throw_error("wrong argument type");
                    }

                    x = (number_t *) left->data;
                    y = (number_t *) right->data;

                    if (subtract_numbers(x[0], y[0], &difference) != 0)
                    {
                        return throw_error("arithmetic error");
                    }

                    result = new_number(difference);
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
            else if (strcmp(data->identifier->name, "multiply") == 0)
            {
                list_node_t *node;
                value_t *left, *right, *result;

                node = data->arguments->head;

                if (!node)
                {
                    return throw_error("absent argument");
                }

                left = apply_statement(node->value, variables);

                if (left->thrown)
                {
                    return left;
                }

                node = node->next;

                if (!node)
                {
                    return throw_error("absent argument");
                }

                right = apply_statement(node->value, variables);

                if (right->thrown)
                {
                    return right;
                }

                if (left->type == VALUE_TYPE_NUMBER)
                {
                    number_t *x, *y, product;

                    if (right->type != VALUE_TYPE_NUMBER)
                    {
                        return throw_error("wrong argument type");
                    }

                    x = (number_t *) left->data;
                    y = (number_t *) right->data;

                    if (multiply_numbers(x[0], y[0], &product) != 0)
                    {
                        return throw_error("arithmetic error");
                    }

                    result = new_number(product);
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
            else if (strcmp(data->identifier->name, "divide") == 0)
            {
                list_node_t *node;
                value_t *left, *right, *result;

                node = data->arguments->head;

                if (!node)
                {
                    return throw_error("absent argument");
                }

                left = apply_statement(node->value, variables);

                if (left->thrown)
                {
                    return left;
                }

                node = node->next;

                if (!node)
                {
                    return throw_error("absent argument");
                }

                right = apply_statement(node->value, variables);

                if (right->thrown)
                {
                    return right;
                }

                if (left->type == VALUE_TYPE_NUMBER)
                {
                    number_t *x, *y, quotient;

                    if (right->type != VALUE_TYPE_NUMBER)
                    {
                        return throw_error("wrong argument type");
                    }

                    x = (number_t *) left->data;
                    y = (number_t *) right->data;

                    if (divide_numbers(x[0], y[0], &quotient) != 0)
                    {
                        return throw_error("arithmetic error");
                    }

                    result = new_number(quotient);
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
            else if (strcmp(data->identifier->name, "merge") == 0)
            {
                list_node_t *node;
                value_t *left, *right, *result;

                node = data->arguments->head;

                if (!node)
                {
                    return throw_error("absent argument");
                }

                left = apply_statement(node->value, variables);

                if (left->thrown)
                {
                    return left;
                }

                node = node->next;

                if (!node)
                {
                    return throw_error("absent argument");
                }

                right = apply_statement(node->value, variables);

                if (right->thrown)
                {
                    return right;
                }

                if (left->type == VALUE_TYPE_STRING)
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
                list_node_t *node;
                value_t *message, *file;
                char *text;

                node = data->arguments->head;

                if (!node)
                {
                    return throw_error("absent argument");
                }

                message = apply_statement(node->value, variables);

                if (message->thrown)
                {
                    return message;
                }

                node = node->next;

                if (!node)
                {
                    return throw_error("absent argument");
                }

                file = apply_statement(node->value, variables);

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
                    number_t streamID, outID, errID;

                    streamID = ((number_t *) file->data)[0];
                    integer_to_number(1, &outID);
                    integer_to_number(2, &errID);

                    if (streamID == outID)
                    {
                        streamHandle = stdout;
                    }
                    else if (streamID == errID)
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
                list_node_t *node;
                value_t *value;

                node = data->arguments->head;

                if (!node)
                {
                    return throw_error("absent argument");
                }

                value = apply_statement(node->value, variables);

                if (value->type == VALUE_TYPE_NUMBER)
                {
                    number_t number;
                    char *string;

                    number = ((number_t *) value->data)[0];
                    string = represent_number(number);

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

static value_t *new_number(number_t number)
{
    value_t *value;
    number_t *data;

    data = allocate(sizeof(number_t));
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
