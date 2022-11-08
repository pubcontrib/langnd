#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "execute.h"
#include "parse.h"
#include "utility.h"

typedef struct
{
    list_t *statements;
    value_t **evaluated;
    list_node_t *current;
    size_t index;
} argument_iterator_t;

static value_t *apply_statement(statement_t *statement, map_t *variables);
static value_t *run_add(argument_iterator_t *arguments, map_t *variables);
static value_t *run_subtract(argument_iterator_t *arguments, map_t *variables);
static value_t *run_multiply(argument_iterator_t *arguments, map_t *variables);
static value_t *run_divide(argument_iterator_t *arguments, map_t *variables);
static value_t *run_modulo(argument_iterator_t *arguments, map_t *variables);
static value_t *run_truncate(argument_iterator_t *arguments, map_t *variables);
static value_t *run_and(argument_iterator_t *arguments, map_t *variables);
static value_t *run_or(argument_iterator_t *arguments, map_t *variables);
static value_t *run_not(argument_iterator_t *arguments, map_t *variables);
static value_t *run_precedes(argument_iterator_t *arguments, map_t *variables);
static value_t *run_succeeds(argument_iterator_t *arguments, map_t *variables);
static value_t *run_equals(argument_iterator_t *arguments, map_t *variables);
static value_t *run_write(argument_iterator_t *arguments, map_t *variables);
static value_t *run_read(argument_iterator_t *arguments, map_t *variables);
static value_t *run_delete(argument_iterator_t *arguments, map_t *variables);
static value_t *run_query(argument_iterator_t *arguments, map_t *variables);
static value_t *run_freeze(argument_iterator_t *arguments, map_t *variables);
static value_t *run_type(argument_iterator_t *arguments, map_t *variables);
static value_t *run_cast(argument_iterator_t *arguments, map_t *variables);
static value_t *run_get(argument_iterator_t *arguments, map_t *variables);
static value_t *run_set(argument_iterator_t *arguments, map_t *variables);
static value_t *run_unset(argument_iterator_t *arguments, map_t *variables);
static value_t *run_merge(argument_iterator_t *arguments, map_t *variables);
static value_t *run_length(argument_iterator_t *arguments, map_t *variables);
static int next_argument(argument_iterator_t *arguments, map_t *variables, int types, value_t **out);
static int has_next_argument(argument_iterator_t *arguments);
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

        if (result->thrown)
        {
            outcome->errorMessage = copy_string("failed to execute code");
            outcome->hintMessage = represent_value(result);
            dereference_value(result);

            break;
        }

        dereference_value(result);

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
        case STATEMENT_TYPE_LITERAL:
        {
            literal_statement_data_t *data;
            value_t *value;

            data = statement->data;
            value = data->value;
            value->owners += 1;

            return value;
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

            if (strcmp(data->identifier->name, "add") == 0)
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
            else if (strcmp(data->identifier->name, "modulo") == 0)
            {
                result = run_modulo(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "truncate") == 0)
            {
                result = run_truncate(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "and") == 0)
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
            else if (strcmp(data->identifier->name, "precedes") == 0)
            {
                result = run_precedes(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "succeeds") == 0)
            {
                result = run_succeeds(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "equals") == 0)
            {
                result = run_equals(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "write") == 0)
            {
                result = run_write(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "read") == 0)
            {
                result = run_read(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "delete") == 0)
            {
                result = run_delete(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "query") == 0)
            {
                result = run_query(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "freeze") == 0)
            {
                result = run_freeze(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "type") == 0)
            {
                result = run_type(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "cast") == 0)
            {
                result = run_cast(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "get") == 0)
            {
                result = run_get(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "set") == 0)
            {
                result = run_set(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "unset") == 0)
            {
                result = run_unset(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "merge") == 0)
            {
                result = run_merge(&arguments, variables);
            }
            else if (strcmp(data->identifier->name, "length") == 0)
            {
                result = run_length(&arguments, variables);
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

            if (test->thrown)
            {
                return test;
            }

            if (test->type == VALUE_TYPE_BOOLEAN)
            {
                value_t *last;
                list_t *body;
                list_node_t *node;

                body = view_boolean(test) ? data->pass : data->fail;
                dereference_value(test);
                last = NULL;

                if (body)
                {
                    for (node = body->head; node; node = node->next)
                    {
                        if (last)
                        {
                            dereference_value(last);
                        }

                        last = apply_statement(node->value, variables);

                        if (last->thrown)
                        {
                            break;
                        }
                    }
                }

                if (!last)
                {
                    last = new_null();
                }

                return last;
            }
            else
            {
                dereference_value(test);

                return throw_error("branch with non-boolean condition");
            }
        }

        case STATEMENT_TYPE_LOOP:
        {
            loop_statement_data_t *data;
            value_t *last;

            data = statement->data;
            last = NULL;

            while (1)
            {
                value_t *test;

                test = apply_statement(data->condition, variables);

                if (test->thrown)
                {
                    return test;
                }

                if (test->type == VALUE_TYPE_BOOLEAN)
                {
                    list_node_t *node;

                    if (!view_boolean(test))
                    {
                        dereference_value(test);
                        break;
                    }

                    dereference_value(test);

                    for (node = data->body->head; node; node = node->next)
                    {
                        if (last)
                        {
                            dereference_value(last);
                        }

                        last = apply_statement(node->value, variables);

                        if (last->thrown)
                        {
                            return last;
                        }
                    }
                }
                else
                {
                    dereference_value(test);

                    return throw_error("branch with non-boolean condition");
                }
            }

            if (!last)
            {
                last = new_null();
            }

            return last;
        }

        case STATEMENT_TYPE_CATCH:
        {
            catch_statement_data_t *data;
            list_node_t *node;

            data = statement->data;

            for (node = data->body->head; node; node = node->next)
            {
                value_t *last;

                last = apply_statement(node->value, variables);

                if (last->thrown)
                {
                    last->thrown = 0;
                    return last;
                }
                else
                {
                    dereference_value(last);
                }
            }

            return new_null();
        }

        case STATEMENT_TYPE_THROW:
        {
            throw_statement_data_t *data;
            value_t *test;

            data = statement->data;
            test = apply_statement(data->error, variables);

            if (test->thrown)
            {
                return test;
            }
            else
            {
                test->thrown = 1;
                return test;
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

static value_t *run_add(argument_iterator_t *arguments, map_t *variables)
{
    value_t *left, *right;
    number_t sum;

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &left))
    {
        return left;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &right))
    {
        return right;
    }

    if (add_numbers(view_number(left), view_number(right), &sum) != 0)
    {
        return throw_error("arithmetic error");
    }

    return new_number(sum);
}

static value_t *run_subtract(argument_iterator_t *arguments, map_t *variables)
{
    value_t *left, *right;
    number_t difference;

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &left))
    {
        return left;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &right))
    {
        return right;
    }

    if (subtract_numbers(view_number(left), view_number(right), &difference) != 0)
    {
        return throw_error("arithmetic error");
    }

    return new_number(difference);
}

static value_t *run_multiply(argument_iterator_t *arguments, map_t *variables)
{
    value_t *left, *right;
    number_t product;

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &left))
    {
        return left;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &right))
    {
        return right;
    }

    if (multiply_numbers(view_number(left), view_number(right), &product) != 0)
    {
        return throw_error("arithmetic error");
    }

    return new_number(product);
}

static value_t *run_divide(argument_iterator_t *arguments, map_t *variables)
{
    value_t *left, *right;
    number_t quotient;

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &left))
    {
        return left;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &right))
    {
        return right;
    }

    if (divide_numbers(view_number(left), view_number(right), &quotient) != 0)
    {
        return throw_error("arithmetic error");
    }

    return new_number(quotient);
}

static value_t *run_modulo(argument_iterator_t *arguments, map_t *variables)
{
    value_t *left, *right;
    number_t remainder;

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &left))
    {
        return left;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &right))
    {
        return right;
    }

    if (modulo_numbers(view_number(left), view_number(right), &remainder) != 0)
    {
        return throw_error("arithmetic error");
    }

    return new_number(remainder);
}

static value_t *run_truncate(argument_iterator_t *arguments, map_t *variables)
{
    value_t *value;

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &value))
    {
        return value;
    }

    return new_number(truncate_number(view_number(value)));
}

static value_t *run_and(argument_iterator_t *arguments, map_t *variables)
{
    value_t *left, *right;

    if (!next_argument(arguments, variables, VALUE_TYPE_BOOLEAN, &left))
    {
        return left;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_BOOLEAN, &right))
    {
        return right;
    }

    return new_boolean(view_boolean(left) && view_boolean(right));
}

static value_t *run_or(argument_iterator_t *arguments, map_t *variables)
{
    value_t *left, *right;

    if (!next_argument(arguments, variables, VALUE_TYPE_BOOLEAN, &left))
    {
        return left;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_BOOLEAN, &right))
    {
        return right;
    }

    return new_boolean(view_boolean(left) || view_boolean(right));
}

static value_t *run_not(argument_iterator_t *arguments, map_t *variables)
{
    value_t *value;

    if (!next_argument(arguments, variables, VALUE_TYPE_BOOLEAN, &value))
    {
        return value;
    }

    return new_boolean(!view_boolean(value));
}

static value_t *run_precedes(argument_iterator_t *arguments, map_t *variables)
{
    value_t *left, *right;

    if (!next_argument(arguments, variables, VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING, &left))
    {
        return left;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING, &right))
    {
        return right;
    }

    return new_boolean(compare_values(left, right) < 0 ? TRUE : FALSE);
}

static value_t *run_succeeds(argument_iterator_t *arguments, map_t *variables)
{
    value_t *left, *right;

    if (!next_argument(arguments, variables, VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING, &left))
    {
        return left;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING, &right))
    {
        return right;
    }

    return new_boolean(compare_values(left, right) > 0 ? TRUE : FALSE);
}

static value_t *run_equals(argument_iterator_t *arguments, map_t *variables)
{
    value_t *left, *right;

    if (!next_argument(arguments, variables, VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING, &left))
    {
        return left;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING, &right))
    {
        return right;
    }

    return new_boolean(compare_values(left, right) == 0 ? TRUE : FALSE);
}

static value_t *run_write(argument_iterator_t *arguments, map_t *variables)
{
    value_t *message, *file;
    FILE *handle;
    int closable, flushable;
    char *text;

    if (!next_argument(arguments, variables, VALUE_TYPE_STRING, &message))
    {
        return message;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER | VALUE_TYPE_STRING, &file))
    {
        return file;
    }

    text = view_string(message);
    closable = 0;
    flushable = 0;

    switch (file->type)
    {
        case VALUE_TYPE_NUMBER:
        {
            number_t streamID, inID, outID, errID;

            streamID = view_number(file);
            integer_to_number(0, &inID);
            integer_to_number(1, &outID);
            integer_to_number(2, &errID);

            if (streamID == inID)
            {
                handle = stdin;
            }
            else if (streamID == outID)
            {
                handle = stdout;
            }
            else if (streamID == errID)
            {
                handle = stderr;
            }
            else
            {
                handle = NULL;
            }

            flushable = 1;

            break;
        }

        case VALUE_TYPE_STRING:
        {
            handle = fopen(view_string(file), "wb");
            closable = 1;

            break;
        }

        default:
            crash_with_message("unsupported branch EXECUTE_WRITE_TYPE");
            return new_null();
    }

    if (!handle)
    {
        return throw_error("absent file");
    }

    fwrite(text, sizeof(char), strlen(text), handle);

    if (ferror(handle))
    {
        if (closable)
        {
            fclose(handle);
        }

        return throw_error("io error");
    }

    if (flushable)
    {
        fflush(handle);
    }

    if (closable)
    {
        fclose(handle);
    }

    return new_null();
}

static value_t *run_read(argument_iterator_t *arguments, map_t *variables)
{
    value_t *file, *until;
    FILE *handle;
    int closable;
    char *text, *terminator;
    size_t fill, length;

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER | VALUE_TYPE_STRING, &file))
    {
        return file;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_NULL | VALUE_TYPE_STRING, &until))
    {
        return until;
    }

    terminator = until->type == VALUE_TYPE_STRING ? view_string(until) : NULL;

    if (terminator && strlen(terminator) != 1)
    {
        return throw_error("invalid terminator");
    }

    closable = 0;

    switch (file->type)
    {
        case VALUE_TYPE_NUMBER:
        {
            number_t streamID, inID, outID, errID;

            streamID = view_number(file);
            integer_to_number(0, &inID);
            integer_to_number(1, &outID);
            integer_to_number(2, &errID);

            if (streamID == inID)
            {
                handle = stdin;
            }
            else if (streamID == outID)
            {
                handle = stdout;
            }
            else if (streamID == errID)
            {
                handle = stderr;
            }
            else
            {
                handle = NULL;
            }

            break;
        }

        case VALUE_TYPE_STRING:
            handle = fopen(view_string(file), "rb");
            closable = 1;

            break;

        default:
            crash_with_message("unsupported branch EXECUTE_READ_TYPE");
            return new_null();
    }

    if (!handle)
    {
        return throw_error("absent file");
    }

    fill = 0;
    length = 256;
    text = allocate(sizeof(char) * (length + 1));

    while (1)
    {
        char symbol;

        symbol = getc(handle);

        if (ferror(handle))
        {
            if (closable)
            {
                fclose(handle);
            }

            free(text);

            return throw_error("io error");
        }

        if (symbol == EOF || (terminator && terminator[0] == symbol))
        {
            text[fill++] = '\0';
            break;
        }

        text[fill++] = symbol;

        if (fill == length)
        {
            char *swap;

            length *= 2;
            swap = allocate(sizeof(char) * (length + 1));
            memcpy(swap, text, fill);
            free(text);
            text = swap;
        }
    }

    if (closable)
    {
        fclose(handle);
    }

    return steal_string(text);
}

static value_t *run_delete(argument_iterator_t *arguments, map_t *variables)
{
    value_t *file;

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER | VALUE_TYPE_STRING, &file))
    {
        return file;
    }

    switch (file->type)
    {
        case VALUE_TYPE_NUMBER:
            return throw_error("io error");

        case VALUE_TYPE_STRING:
        {
            char *fileName;

            fileName = view_string(file);

            if (remove(fileName) == -1)
            {
                return throw_error("io error");
            }

            return new_null();
        }

        default:
            crash_with_message("unsupported branch EXECUTE_DELETE_TYPE");
            return new_null();
    }
}

static value_t *run_query(argument_iterator_t *arguments, map_t *variables)
{
    value_t *key;
    char *value;

    if (!next_argument(arguments, variables, VALUE_TYPE_STRING, &key))
    {
        return key;
    }

    value = getenv(view_string(key));
    return value ? new_string(value) : throw_error("absent environment variable");
}

static value_t *run_freeze(argument_iterator_t *arguments, map_t *variables)
{
    value_t *value;

    if (!next_argument(arguments, variables, VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING, &value))
    {
        return value;
    }

    return steal_string(represent_value(value));
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

    pattern = view_string(type);

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

            string = view_string(value);

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

            string = view_string(value);

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
            return view_boolean(value) == TRUE ? new_string("true") : new_string("false");
        }
        else if (value->type == VALUE_TYPE_NUMBER)
        {
            return steal_string(represent_number(view_number(value)));
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

static value_t *run_get(argument_iterator_t *arguments, map_t *variables)
{
    value_t *collection, *key;

    if (!next_argument(arguments, variables, VALUE_TYPE_STRING, &collection))
    {
        return collection;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &key))
    {
        return key;
    }

    switch (collection->type)
    {
        case VALUE_TYPE_STRING:
        {
            char *string;
            int index;
            size_t length;
            char item[2];

            string = view_string(collection);

            if (number_to_integer(view_number(key), &index) != 0)
            {
                crash_with_message("unsupported branch EXECUTE_GET_INDEX");
            }

            length = strlen(string);

            if (index < 1 || (size_t) index > length)
            {
                return throw_error("absent key");
            }

            item[0] = string[index - 1];
            item[1] = '\0';

            return new_string(item);
        }

        default:
            crash_with_message("unsupported branch EXECUTE_GET_TYPE");
            return new_null();
    }
}

static value_t *run_set(argument_iterator_t *arguments, map_t *variables)
{
    value_t *collection, *key, *item;

    if (!next_argument(arguments, variables, VALUE_TYPE_STRING, &collection))
    {
        return collection;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &key))
    {
        return key;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_STRING, &item))
    {
        return item;
    }

    switch (collection->type)
    {
        case VALUE_TYPE_STRING:
        {
            char *source, *middle, *destination;
            int index;
            size_t sourceLength, beforeLength, middleLength, afterLength, destinationLength;

            source = view_string(collection);
            middle = view_string(item);

            if (number_to_integer(view_number(key), &index) != 0)
            {
                crash_with_message("unsupported branch EXECUTE_SET_INDEX");
            }

            sourceLength = strlen(source);

            if (index < 1 || (size_t) index > sourceLength)
            {
                return throw_error("absent key");
            }

            beforeLength = index - 1;
            middleLength = strlen(middle);
            afterLength = sourceLength - beforeLength - 1;
            destinationLength = beforeLength + middleLength + afterLength;

            destination = allocate(sizeof(char) * (destinationLength + 1));
            memcpy(destination, source, beforeLength);
            memcpy(destination + beforeLength, middle, middleLength);
            memcpy(destination + beforeLength + middleLength, source + beforeLength + 1, afterLength);
            destination[destinationLength] = '\0';

            return steal_string(destination);
        }

        default:
            crash_with_message("unsupported branch EXECUTE_SET_TYPE");
            return new_null();
    }
}

static value_t *run_unset(argument_iterator_t *arguments, map_t *variables)
{
    value_t *collection, *key;

    if (!next_argument(arguments, variables, VALUE_TYPE_STRING, &collection))
    {
        return collection;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &key))
    {
        return key;
    }

    switch (collection->type)
    {
        case VALUE_TYPE_STRING:
        {
            char *source, *destination;
            int index;
            size_t length;

            source = view_string(collection);

            if (number_to_integer(view_number(key), &index) != 0)
            {
                crash_with_message("unsupported branch EXECUTE_UNSET_INDEX");
            }

            length = strlen(source);

            if (index < 1 || (size_t) index > length)
            {
                return throw_error("absent key");
            }

            destination = allocate(sizeof(char) * length);
            memcpy(destination, source, index - 1);
            memcpy(destination + index - 1, source + index, length - index);
            destination[length - 1] = '\0';

            return steal_string(destination);
        }

        default:
            crash_with_message("unsupported branch EXECUTE_SET_TYPE");
            return new_null();
    }
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

    x = view_string(left);
    y = view_string(right);
    length = strlen(x) + strlen(y);
    sum = allocate(sizeof(char) * length + 1);
    memcpy(sum, x, strlen(x));
    memcpy(sum + strlen(x), y, strlen(y));
    sum[length] = '\0';
    result = steal_string(sum);

    return result;
}

static value_t *run_length(argument_iterator_t *arguments, map_t *variables)
{
    value_t *collection;

    if (!next_argument(arguments, variables, VALUE_TYPE_STRING, &collection))
    {
        return collection;
    }

    switch (collection->type)
    {
        case VALUE_TYPE_STRING:
        {
            size_t length;
            number_t number;

            length = strlen(view_string(collection));

            if (length >= INT_MAX || integer_to_number(length, &number) != 0)
            {
                return throw_error("constraint error");
            }

            return new_number(number);
        }

        default:
            crash_with_message("unsupported branch EXECUTE_LENGTH_TYPE");
            return new_null();
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

static void dereference_value_unsafe(void *value)
{
    dereference_value(value);
}
