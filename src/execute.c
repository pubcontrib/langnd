#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "execute.h"
#include "parse.h"
#include "utility.h"

typedef enum
{
    VALUE_TYPE_NULL = 0x01,
    VALUE_TYPE_BOOLEAN = 0x02,
    VALUE_TYPE_NUMBER = 0x04,
    VALUE_TYPE_STRING = 0x08
} value_type_t;

typedef struct
{
    value_type_t type;
    void *data;
    int thrown;
    int owners;
} value_t;

typedef struct
{
    list_t *statements;
    value_t **evaluated;
    list_node_t *current;
    size_t index;
} argument_iterator_t;

static value_t *apply_statement(statement_t *statement, map_t *variables);
static value_t *run_and(argument_iterator_t *arguments, map_t *variables);
static value_t *run_or(argument_iterator_t *arguments, map_t *variables);
static value_t *run_not(argument_iterator_t *arguments, map_t *variables);
static value_t *run_add(argument_iterator_t *arguments, map_t *variables);
static value_t *run_subtract(argument_iterator_t *arguments, map_t *variables);
static value_t *run_multiply(argument_iterator_t *arguments, map_t *variables);
static value_t *run_divide(argument_iterator_t *arguments, map_t *variables);
static value_t *run_merge(argument_iterator_t *arguments, map_t *variables);
static value_t *run_write(argument_iterator_t *arguments, map_t *variables);
static value_t *run_type(argument_iterator_t *arguments, map_t *variables);
static value_t *run_cast(argument_iterator_t *arguments, map_t *variables);
static int next_argument(argument_iterator_t *arguments, map_t *variables, int types, value_t **out);
static int has_next_argument(argument_iterator_t *arguments);
static value_t *throw_error(char *message);
static value_t *new_null();
static value_t *new_boolean(boolean_t boolean);
static value_t *new_number(number_t number);
static value_t *new_string(char *string);
static value_t *steal_string(char *string);
static void destroy_value(value_t *value);
static void dereference_value(value_t *value);
static void dereference_value_unsafe(void *value);

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

    variables = empty_map(hash_string, dereference_value_unsafe, 8);
    node = script->statements->head;

    while (node)
    {
        value_t *result;

        result = apply_statement(node->value, variables);
        result->owners -= 1;

        if (result->thrown)
        {
            if (result->type == VALUE_TYPE_STRING)
            {
                char *message;

                message = copy_string(result->data);

                if (result->owners < 1)
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

        if (result->owners < 1)
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
        case STATEMENT_TYPE_NULL:
        {
            return new_null();
        }

        case STATEMENT_TYPE_BOOLEAN:
        {
            boolean_statement_data_t *data;

            data = statement->data;

            return new_boolean(data->value);
        }

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

            set_map_item(variables, copy_string(data->identifier->name), value);

            return new_null();
        }

        case STATEMENT_TYPE_INVOKE:
        {
            invoke_statement_data_t *data;
            argument_iterator_t arguments;
            value_t *result;

            data = statement->data;
            arguments.statements = data->arguments;

            if (data->arguments->length > 0)
            {
                arguments.current = data->arguments->head;
                arguments.evaluated = allocate_with_zeros(sizeof(value_t *), data->arguments->length);
            }
            else
            {
                arguments.current = NULL;
                arguments.evaluated = NULL;
            }

            arguments.index = 0;

            if (strcmp(data->identifier->name, "and") == 0)
            {
                result = run_and(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "or") == 0)
            {
                result = run_or(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "not") == 0)
            {
                result = run_not(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "add") == 0)
            {
                result = run_add(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "subtract") == 0)
            {
                result = run_subtract(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "multiply") == 0)
            {
                result = run_multiply(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "divide") == 0)
            {
                result = run_divide(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "merge") == 0)
            {
                result = run_merge(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "write") == 0)
            {
                result = run_write(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "type") == 0)
            {
                result = run_type(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "cast") == 0)
            {
                result = run_cast(&arguments, variables);
            }
            else
            {
                result = throw_error("absent function");
            }

            if (arguments.evaluated)
            {
                size_t index;

                for (index = 0; index < arguments.index; index++)
                {
                    dereference_value(arguments.evaluated[index]);
                }

                free(arguments.evaluated);
            }

            return result;
        }

        case STATEMENT_TYPE_BRANCH:
        {
            branch_statement_data_t *data;
            value_t *test;

            data = statement->data;
            test = apply_statement(data->condition, variables);

            if (test->type == VALUE_TYPE_BOOLEAN)
            {
                if (((char *) test->data)[0])
                {
                    value_t *last;
                    list_node_t *node;

                    last = NULL;

                    for (node = data->body->head; node; node = node->next)
                    {
                        if (last)
                        {
                            destroy_value(last);
                        }

                        last = apply_statement(node->value, variables);

                        if (last->thrown)
                        {
                            break;
                        }
                    }

                    dereference_value(test);

                    return last;
                }
                else
                {
                    dereference_value(test);

                    return new_null();
                }
            }
            else
            {
                dereference_value(test);

                return throw_error("branch with non-boolean condition");
            }
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
                    value->owners += 1;
                    return value;
                }
                else
                {
                    return throw_error("absent variable");
                }
            }
            else if (data->identifier->type == IDENTIFIER_TYPE_FUNCTION)
            {
                return throw_error("unexpected reference type");
            }
        }

        default:
            break;
    }

    return new_null();
}

static value_t *run_and(argument_iterator_t *arguments, map_t *variables)
{
    value_t *left, *right;
    boolean_t *x, *y;

    if (!next_argument(arguments, variables, VALUE_TYPE_BOOLEAN, &left))
    {
        return left;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_BOOLEAN, &right))
    {
        return right;
    }

    x = (boolean_t *) left->data;
    y = (boolean_t *) right->data;

    return new_boolean(x[0] && y[0]);
}

static value_t *run_or(argument_iterator_t *arguments, map_t *variables)
{
    value_t *left, *right;
    boolean_t *x, *y;

    if (!next_argument(arguments, variables, VALUE_TYPE_BOOLEAN, &left))
    {
        return left;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_BOOLEAN, &right))
    {
        return right;
    }

    x = (boolean_t *) left->data;
    y = (boolean_t *) right->data;

    return new_boolean(x[0] || y[0]);
}

static value_t *run_not(argument_iterator_t *arguments, map_t *variables)
{
    value_t *value;
    boolean_t *i;

    if (!next_argument(arguments, variables, VALUE_TYPE_BOOLEAN, &value))
    {
        return value;
    }

    i = (boolean_t *) value->data;

    return new_boolean(!i[0]);
}

static value_t *run_add(argument_iterator_t *arguments, map_t *variables)
{
    value_t *left, *right;
    number_t *x, *y, sum;

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &left))
    {
        return left;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &right))
    {
        return right;
    }

    x = (number_t *) left->data;
    y = (number_t *) right->data;

    if (add_numbers(x[0], y[0], &sum) != 0)
    {
        return throw_error("arithmetic error");
    }

    return new_number(sum);
}

static value_t *run_subtract(argument_iterator_t *arguments, map_t *variables)
{
    value_t *left, *right;
    number_t *x, *y, difference;

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &left))
    {
        return left;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &right))
    {
        return right;
    }

    x = (number_t *) left->data;
    y = (number_t *) right->data;

    if (subtract_numbers(x[0], y[0], &difference) != 0)
    {
        return throw_error("arithmetic error");
    }

    return new_number(difference);
}

static value_t *run_multiply(argument_iterator_t *arguments, map_t *variables)
{
    value_t *left, *right;
    number_t *x, *y, product;

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &left))
    {
        return left;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &right))
    {
        return right;
    }

    x = (number_t *) left->data;
    y = (number_t *) right->data;

    if (multiply_numbers(x[0], y[0], &product) != 0)
    {
        return throw_error("arithmetic error");
    }

    return new_number(product);
}

static value_t *run_divide(argument_iterator_t *arguments, map_t *variables)
{
    value_t *left, *right;
    number_t *x, *y, quotient;

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &left))
    {
        return left;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &right))
    {
        return right;
    }

    x = (number_t *) left->data;
    y = (number_t *) right->data;

    if (divide_numbers(x[0], y[0], &quotient) != 0)
    {
        return throw_error("arithmetic error");
    }

    return new_number(quotient);
}

static value_t *run_merge(argument_iterator_t *arguments, map_t *variables)
{
    value_t *left, *right, *result;
    char *x, *y, *sum;
    size_t length;

    if (!next_argument(arguments, variables, VALUE_TYPE_STRING, &left))
    {
        return left;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_STRING, &right))
    {
        return right;
    }

    x = (char *) left->data;
    y = (char *) right->data;
    length = strlen(x) + strlen(y);
    sum = allocate(sizeof(char) * length + 1);
    memcpy(sum, x, strlen(x));
    memcpy(sum + strlen(x), y, strlen(y));
    sum[length] = '\0';
    result = steal_string(sum);

    return result;
}

static value_t *run_write(argument_iterator_t *arguments, map_t *variables)
{
    value_t *message, *file;
    char *text;

    if (!next_argument(arguments, variables, VALUE_TYPE_STRING, &message))
    {
        return message;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER | VALUE_TYPE_STRING, &file))
    {
        return file;
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
        crash_with_message("unsupported branch EXECUTE_WRITE_TYPE");
        return new_null();
    }
}

static value_t *run_type(argument_iterator_t *arguments, map_t *variables)
{
    value_t *value;

    if (!next_argument(arguments, variables, VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING, &value))
    {
        return value;
    }

    switch (value->type)
    {
        case VALUE_TYPE_NULL:
            return new_string("NULL");

        case VALUE_TYPE_BOOLEAN:
            return new_string("BOOLEAN");

        case VALUE_TYPE_NUMBER:
            return new_string("NUMBER");

        case VALUE_TYPE_STRING:
            return new_string("STRING");

        default:
            crash_with_message("unsupported branch EXECUTE_TYPE_TYPE");
            return new_null();
    }
}

static value_t *run_cast(argument_iterator_t *arguments, map_t *variables)
{
    value_t *value, *type;
    char *pattern;

    if (!next_argument(arguments, variables, VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING, &value))
    {
        return value;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_STRING, &type))
    {
        return type;
    }

    pattern = (char *) type->data;

    if (strcmp(pattern, "NULL") == 0)
    {
        if (value->type == VALUE_TYPE_NULL)
        {
            value->owners += 1;

            return value;
        }
        else if (value->type == VALUE_TYPE_STRING)
        {
            char *string;

            string = (char *) value->data;

            if (strcmp(string, "null") == 0)
            {
                return new_null();
            }
            else
            {
                return throw_error("invalid cast");
            }
        }
        else
        {
            return throw_error("invalid cast");
        }
    }
    else if (strcmp(pattern, "BOOLEAN") == 0)
    {
        if (value->type == VALUE_TYPE_BOOLEAN)
        {
            value->owners += 1;

            return value;
        }
        else if (value->type == VALUE_TYPE_STRING)
        {
            char *string;

            string = (char *) value->data;

            if (strcmp(string, "false") == 0)
            {
                return new_boolean(FALSE);
            }
            else if (strcmp(string, "true") == 0)
            {
                return new_boolean(TRUE);
            }
            else
            {
                return throw_error("invalid cast");
            }
        }
        else
        {
            return throw_error("invalid cast");
        }
    }
    else if (strcmp(pattern, "NUMBER") == 0)
    {
        if (value->type == VALUE_TYPE_NUMBER)
        {
            value->owners += 1;

            return value;
        }
        else if (value->type == VALUE_TYPE_STRING)
        {
            number_t number;

            if (string_to_number(value->data, &number) != 0)
            {
                return throw_error("invalid cast");
            }

            return new_number(number);
        }
        else
        {
            return throw_error("invalid cast");
        }
    }
    else if (strcmp(pattern, "STRING") == 0)
    {
        if (value->type == VALUE_TYPE_NULL)
        {
            return new_string("null");
        }
        else if (value->type == VALUE_TYPE_BOOLEAN)
        {
            value_t *result;
            boolean_t boolean;

            boolean = ((boolean_t *) value->data)[0];
            result = boolean == TRUE ? new_string("true") : new_string("false");

            return result;
        }
        else if (value->type == VALUE_TYPE_NUMBER)
        {
            value_t *result;
            number_t number;
            char *string;

            number = ((number_t *) value->data)[0];
            string = represent_number(number);
            result = steal_string(string);

            return result;
        }
        else if (value->type == VALUE_TYPE_STRING)
        {
            value->owners += 1;

            return value;
        }
        else
        {
            return throw_error("invalid cast");
        }
    }
    else
    {
        return throw_error("unknown type");
    }
}

static int next_argument(argument_iterator_t *arguments, map_t *variables, int types, value_t **out)
{
    value_t *result;

    if (!has_next_argument(arguments))
    {
        (*out) = throw_error("absent argument");
        return 0;
    }

    result = apply_statement(arguments->current->value, variables);
    arguments->current = arguments->current->next;
    arguments->evaluated[arguments->index] = result;
    arguments->index += 1;

    if (result->thrown)
    {
        result->owners += 1;
        (*out) = result;
        return 0;
    }

    if (!(types & result->type))
    {
        (*out) = throw_error("wrong argument type");
        return 0;
    }

    (*out) = result;
    return 1;
}

static int has_next_argument(argument_iterator_t *arguments)
{
    return arguments->index < arguments->statements->length;
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
    value->owners = 1;

    return value;
}

static value_t *new_boolean(boolean_t boolean)
{
    value_t *value;
    boolean_t *data;

    data = allocate(sizeof(boolean_t));
    data[0] = boolean;
    value = allocate(sizeof(value_t));
    value->type = VALUE_TYPE_BOOLEAN;
    value->data = data;
    value->thrown = 0;
    value->owners = 1;

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
    value->owners = 1;

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
    value->owners = 1;

    return value;
}

static value_t *steal_string(char *data)
{
    value_t *value;

    value = allocate(sizeof(value_t));
    value->type = VALUE_TYPE_STRING;
    value->data = data;
    value->thrown = 0;
    value->owners = 1;

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

static void dereference_value(value_t *value)
{
    value->owners -= 1;

    if (value->owners < 1)
    {
        destroy_value(value);
    }
}

static void dereference_value_unsafe(void *value)
{
    dereference_value(value);
}
