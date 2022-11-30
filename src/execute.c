#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "execute.h"
#include "parse.h"
#include "utility.h"

typedef struct
{
    list_t *statements;
    value_t **evaluated;
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
static value_t *run_keys(argument_iterator_t *arguments, map_t *variables);
static value_t *run_sort(argument_iterator_t *arguments, map_t *variables);
static int next_argument(argument_iterator_t *arguments, map_t *variables, int types, value_t **out);
static int has_next_argument(argument_iterator_t *arguments);
static void copy_map_items(map_t *source, map_t *destination);
static int compare_values_ascending(const void *left, const void *right);
static int compare_values_descending(const void *left, const void *right);
static void dereference_value_unsafe(void *value);

outcome_t *execute(string_t *code)
{
    outcome_t *outcome;
    script_t *script;
    map_t *variables;
    list_t *statements;
    size_t index;

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
    statements = script->statements;

    for (index = 0; index < statements->length; index++)
    {
        value_t *result;

        result = apply_statement(statements->items[index], variables);

        if (result->thrown)
        {
            outcome->errorMessage = cstring_to_string("failed to execute code");
            outcome->hintMessage = represent_value(result);
            dereference_value(result);

            break;
        }

        dereference_value(result);
    }

    destroy_map(variables);
    destroy_script(script);

    return outcome;
}

void destroy_outcome(outcome_t *outcome)
{
    if (outcome->errorMessage)
    {
        destroy_string(outcome->errorMessage);
    }

    if (outcome->hintMessage)
    {
        destroy_string(outcome->hintMessage);
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
                arguments.evaluated = allocate_with_zeros(sizeof(value_t *), data->arguments->length);
            }
            else
            {
                arguments.evaluated = NULL;
            }

            arguments.index = 0;

            if (is_keyword_match(data->identifier->name, "add"))
            {
                result = run_add(&arguments, variables);
            }
            else if (is_keyword_match(data->identifier->name, "subtract"))
            {
                result = run_subtract(&arguments, variables);
            }
            else if (is_keyword_match(data->identifier->name, "multiply"))
            {
                result = run_multiply(&arguments, variables);
            }
            else if (is_keyword_match(data->identifier->name, "divide"))
            {
                result = run_divide(&arguments, variables);
            }
            else if (is_keyword_match(data->identifier->name, "modulo"))
            {
                result = run_modulo(&arguments, variables);
            }
            else if (is_keyword_match(data->identifier->name, "truncate"))
            {
                result = run_truncate(&arguments, variables);
            }
            else if (is_keyword_match(data->identifier->name, "and"))
            {
                result = run_and(&arguments, variables);
            }
            else if (is_keyword_match(data->identifier->name, "or"))
            {
                result = run_or(&arguments, variables);
            }
            else if (is_keyword_match(data->identifier->name, "not"))
            {
                result = run_not(&arguments, variables);
            }
            else if (is_keyword_match(data->identifier->name, "precedes"))
            {
                result = run_precedes(&arguments, variables);
            }
            else if (is_keyword_match(data->identifier->name, "succeeds"))
            {
                result = run_succeeds(&arguments, variables);
            }
            else if (is_keyword_match(data->identifier->name, "equals"))
            {
                result = run_equals(&arguments, variables);
            }
            else if (is_keyword_match(data->identifier->name, "write"))
            {
                result = run_write(&arguments, variables);
            }
            else if (is_keyword_match(data->identifier->name, "read"))
            {
                result = run_read(&arguments, variables);
            }
            else if (is_keyword_match(data->identifier->name, "delete"))
            {
                result = run_delete(&arguments, variables);
            }
            else if (is_keyword_match(data->identifier->name, "query"))
            {
                result = run_query(&arguments, variables);
            }
            else if (is_keyword_match(data->identifier->name, "freeze"))
            {
                result = run_freeze(&arguments, variables);
            }
            else if (is_keyword_match(data->identifier->name, "type"))
            {
                result = run_type(&arguments, variables);
            }
            else if (is_keyword_match(data->identifier->name, "cast"))
            {
                result = run_cast(&arguments, variables);
            }
            else if (is_keyword_match(data->identifier->name, "get"))
            {
                result = run_get(&arguments, variables);
            }
            else if (is_keyword_match(data->identifier->name, "set"))
            {
                result = run_set(&arguments, variables);
            }
            else if (is_keyword_match(data->identifier->name, "unset"))
            {
                result = run_unset(&arguments, variables);
            }
            else if (is_keyword_match(data->identifier->name, "merge"))
            {
                result = run_merge(&arguments, variables);
            }
            else if (is_keyword_match(data->identifier->name, "length"))
            {
                result = run_length(&arguments, variables);
            }
            else if (is_keyword_match(data->identifier->name, "keys"))
            {
                result = run_keys(&arguments, variables);
            }
            else if (is_keyword_match(data->identifier->name, "sort"))
            {
                result = run_sort(&arguments, variables);
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
                size_t index;

                body = view_boolean(test) ? data->pass : data->fail;
                dereference_value(test);
                last = NULL;

                if (body)
                {
                    for (index = 0; index < body->length; index++)
                    {
                        if (last)
                        {
                            dereference_value(last);
                        }

                        last = apply_statement(body->items[index], variables);

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
                    list_t *body;
                    size_t index;

                    if (!view_boolean(test))
                    {
                        dereference_value(test);
                        break;
                    }

                    dereference_value(test);
                    body = data->body;

                    for (index = 0; index < body->length; index++)
                    {
                        if (last)
                        {
                            dereference_value(last);
                        }

                        last = apply_statement(body->items[index], variables);

                        if (last->thrown)
                        {
                            return last;
                        }
                    }
                }
                else
                {
                    dereference_value(test);

                    return throw_error("loop with non-boolean condition");
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
            list_t *body;
            size_t index;

            data = statement->data;
            body = data->body;

            for (index = 0; index < body->length; index++)
            {
                value_t *last;

                last = apply_statement(body->items[index], variables);

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

    if (!next_argument(arguments, variables, VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP, &left))
    {
        return left;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP, &right))
    {
        return right;
    }

    return new_boolean(compare_values(left, right) < 0 ? TRUE : FALSE);
}

static value_t *run_succeeds(argument_iterator_t *arguments, map_t *variables)
{
    value_t *left, *right;

    if (!next_argument(arguments, variables, VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP, &left))
    {
        return left;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP, &right))
    {
        return right;
    }

    return new_boolean(compare_values(left, right) > 0 ? TRUE : FALSE);
}

static value_t *run_equals(argument_iterator_t *arguments, map_t *variables)
{
    value_t *left, *right;

    if (!next_argument(arguments, variables, VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP, &left))
    {
        return left;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP, &right))
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
    string_t *string;

    if (!next_argument(arguments, variables, VALUE_TYPE_STRING, &message))
    {
        return message;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER | VALUE_TYPE_STRING, &file))
    {
        return file;
    }

    string = view_string(message);
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
            char *cstring;

            cstring = string_to_cstring(view_string(file));
            handle = fopen(cstring, "wb");
            free(cstring);
            closable = 1;

            break;
        }

        default:
            crash_with_message("unsupported branch invoked");
            return new_null();
    }

    if (!handle)
    {
        return throw_error("absent file");
    }

    fwrite(string->bytes, sizeof(char), string->length, handle);

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
    int closable, terminated;
    string_t *terminator;
    char *bytes;
    size_t fill, length;

    if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER | VALUE_TYPE_STRING, &file))
    {
        return file;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_NULL | VALUE_TYPE_STRING, &until))
    {
        return until;
    }

    terminated = until->type == VALUE_TYPE_STRING;
    terminator = terminated ? view_string(until) : NULL;

    if (terminated && terminator->length != 1)
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
        {
            char *cstring;

            cstring = string_to_cstring(view_string(file));
            handle = fopen(cstring, "rb");
            free(cstring);
            closable = 1;

            break;
        }

        default:
            crash_with_message("unsupported branch invoked");
            return new_null();
    }

    if (!handle)
    {
        return throw_error("absent file");
    }

    fill = 0;
    length = 256;
    bytes = allocate(sizeof(char) * length);

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

            free(bytes);

            return throw_error("io error");
        }

        if (symbol == EOF || (terminated && terminator->bytes[0] == symbol))
        {
            break;
        }

        bytes[fill++] = symbol;

        if (fill == length)
        {
            char *swap;

            length *= 2;
            swap = allocate(sizeof(char) * length);
            memcpy(swap, bytes, fill);
            free(bytes);
            bytes = swap;
        }
    }

    if (closable)
    {
        fclose(handle);
    }

    return steal_string(create_string(bytes, fill));
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
            char *cstring;
            int status;

            cstring = string_to_cstring(view_string(file));
            status = remove(cstring);
            free(cstring);

            return status != -1 ? new_null() : throw_error("io error");
        }

        default:
            crash_with_message("unsupported branch invoked");
            return new_null();
    }
}

static value_t *run_query(argument_iterator_t *arguments, map_t *variables)
{
    value_t *key;
    char *value, *cstring;

    if (!next_argument(arguments, variables, VALUE_TYPE_STRING, &key))
    {
        return key;
    }

    cstring = string_to_cstring(view_string(key));
    value = getenv(cstring);
    free(cstring);

    return value ? steal_string(cstring_to_string(value)) : throw_error("absent environment variable");
}

static value_t *run_freeze(argument_iterator_t *arguments, map_t *variables)
{
    value_t *value;

    if (!next_argument(arguments, variables, VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP, &value))
    {
        return value;
    }

    return steal_string(represent_value(value));
}

static value_t *run_type(argument_iterator_t *arguments, map_t *variables)
{
    value_t *value;

    if (!next_argument(arguments, variables, VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP, &value))
    {
        return value;
    }

    switch (value->type)
    {
        case VALUE_TYPE_NULL:
            return steal_string(cstring_to_string("NULL"));

        case VALUE_TYPE_BOOLEAN:
            return steal_string(cstring_to_string("BOOLEAN"));

        case VALUE_TYPE_NUMBER:
            return steal_string(cstring_to_string("NUMBER"));

        case VALUE_TYPE_STRING:
            return steal_string(cstring_to_string("STRING"));

        case VALUE_TYPE_LIST:
            return steal_string(cstring_to_string("LIST"));

        case VALUE_TYPE_MAP:
            return steal_string(cstring_to_string("MAP"));

        default:
            crash_with_message("unsupported branch invoked");
            return new_null();
    }
}

static value_t *run_cast(argument_iterator_t *arguments, map_t *variables)
{
    value_t *value, *type;
    string_t *pattern;

    if (!next_argument(arguments, variables, VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP, &value))
    {
        return value;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_STRING, &type))
    {
        return type;
    }

    pattern = view_string(type);

    if (is_keyword_match(pattern, "NULL"))
    {
        if (value->type == VALUE_TYPE_NULL)
        {
            value->owners += 1;

            return value;
        }
        else if (value->type == VALUE_TYPE_STRING)
        {
            string_t *string;

            string = view_string(value);

            if (is_keyword_match(string, "null"))
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
    else if (is_keyword_match(pattern, "BOOLEAN"))
    {
        if (value->type == VALUE_TYPE_BOOLEAN)
        {
            value->owners += 1;

            return value;
        }
        else if (value->type == VALUE_TYPE_STRING)
        {
            string_t *string;

            string = view_string(value);

            if (is_keyword_match(string, "false"))
            {
                return new_boolean(FALSE);
            }
            else if (is_keyword_match(string, "true"))
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
    else if (is_keyword_match(pattern, "NUMBER"))
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
    else if (is_keyword_match(pattern, "STRING"))
    {
        if (value->type == VALUE_TYPE_NULL)
        {
            return steal_string(cstring_to_string("null"));
        }
        else if (value->type == VALUE_TYPE_BOOLEAN)
        {
            return view_boolean(value) == TRUE ? steal_string(cstring_to_string("true")) : steal_string(cstring_to_string("false"));
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
    else if (is_keyword_match(pattern, "LIST"))
    {
        return throw_error("invalid cast");
    }
    else if (is_keyword_match(pattern, "MAP"))
    {
        return throw_error("invalid cast");
    }
    else
    {
        return throw_error("unknown type");
    }
}

static value_t *run_get(argument_iterator_t *arguments, map_t *variables)
{
    value_t *collection, *key;

    if (!next_argument(arguments, variables, VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP, &collection))
    {
        return collection;
    }

    switch (collection->type)
    {
        case VALUE_TYPE_STRING:
        {
            char *bytes;
            string_t *string;
            int number;

            if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &key))
            {
                return key;
            }

            if (number_to_integer(view_number(key), &number) != 0)
            {
                crash_with_message("unsupported branch invoked");
            }

            string = view_string(collection);

            if (number < 1 || (size_t) number > string->length)
            {
                return throw_error("absent key");
            }

            bytes = allocate(sizeof(char));
            bytes[0] = string->bytes[number - 1];

            return steal_string(create_string(bytes, 1));
        }

        case VALUE_TYPE_LIST:
        {
            list_t *list;
            size_t index, cursor;
            int number;

            if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &key))
            {
                return key;
            }

            if (number_to_integer(view_number(key), &number) != 0)
            {
                crash_with_message("unsupported branch invoked");
            }

            list = view_list(collection);

            if (number < 1 || (size_t) number > list->length)
            {
                return throw_error("absent key");
            }

            index = number - 1;

            for (cursor = 0; cursor < list->length; cursor++)
            {
                if (index == cursor)
                {
                    value_t *item;

                    item = list->items[cursor];
                    item->owners += 1;

                    return item;
                }
            }

            crash_with_message("unsupported branch invoked");
            return new_null();
        }

        case VALUE_TYPE_MAP:
        {
            value_t *value;
            map_t *map;

            if (!next_argument(arguments, variables, VALUE_TYPE_STRING, &key))
            {
                return key;
            }

            map = view_map(collection);
            value = get_map_item(map, view_string(key));

            if (value)
            {
                value->owners += 1;
                return value;
            }
            else
            {
                return throw_error("absent key");
            }
        }

        default:
            crash_with_message("unsupported branch invoked");
            return new_null();
    }
}

static value_t *run_set(argument_iterator_t *arguments, map_t *variables)
{
    value_t *collection, *key, *item;

    if (!next_argument(arguments, variables, VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP, &collection))
    {
        return collection;
    }

    switch (collection->type)
    {
        case VALUE_TYPE_STRING:
        {
            string_t *source, *middle;
            char *destination;
            int index;
            size_t beforeLength, afterLength, destinationLength;

            if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &key))
            {
                return key;
            }

            if (!next_argument(arguments, variables, VALUE_TYPE_STRING, &item))
            {
                return item;
            }

            source = view_string(collection);
            middle = view_string(item);

            if (number_to_integer(view_number(key), &index) != 0)
            {
                crash_with_message("unsupported branch invoked");
            }

            if (index < 1 || (size_t) index > source->length)
            {
                return throw_error("absent key");
            }

            beforeLength = index - 1;
            afterLength = source->length - beforeLength - 1;
            destinationLength = beforeLength + middle->length + afterLength;

            destination = allocate(sizeof(char) * destinationLength);
            memcpy(destination, source->bytes, beforeLength);
            memcpy(destination + beforeLength, middle->bytes, middle->length);
            memcpy(destination + beforeLength + middle->length, source->bytes + beforeLength + 1, afterLength);

            return steal_string(create_string(destination, destinationLength));
        }

        case VALUE_TYPE_LIST:
        {
            list_t *source, *destination;
            size_t index, cursor;
            int number;

            if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &key))
            {
                return key;
            }

            if (!next_argument(arguments, variables, VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP, &item))
            {
                return item;
            }

            source = view_list(collection);

            if (number_to_integer(view_number(key), &number) != 0)
            {
                crash_with_message("unsupported branch invoked");
            }

            if (number < 1 || (size_t) number > source->length)
            {
                return throw_error("absent key");
            }

            index = number - 1;
            destination = empty_list(dereference_value_unsafe, source->capacity);

            for (cursor = 0; cursor < source->length; cursor++)
            {
                if (index != cursor)
                {
                    value_t *copy;

                    copy = source->items[cursor];
                    copy->owners += 1;

                    add_list_item(destination, copy);
                }
                else
                {
                    item->owners += 1;

                    add_list_item(destination, item);
                }
            }

            return steal_list(destination);
        }

        case VALUE_TYPE_MAP:
        {
            map_t *source, *destination;

            if (!next_argument(arguments, variables, VALUE_TYPE_STRING, &key))
            {
                return key;
            }

            if (!next_argument(arguments, variables, VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP, &item))
            {
                return item;
            }

            source = view_map(collection);
            destination = empty_map(hash_string, dereference_value_unsafe, source->capacity);

            copy_map_items(source, destination);

            item->owners += 1;
            set_map_item(destination, copy_string(view_string(key)), item);

            return steal_map(destination);
        }

        default:
            crash_with_message("unsupported branch invoked");
            return new_null();
    }
}

static value_t *run_unset(argument_iterator_t *arguments, map_t *variables)
{
    value_t *collection, *key;

    if (!next_argument(arguments, variables, VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP, &collection))
    {
        return collection;
    }

    switch (collection->type)
    {
        case VALUE_TYPE_STRING:
        {
            string_t *source;
            int number;

            if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &key))
            {
                return key;
            }

            if (number_to_integer(view_number(key), &number) != 0)
            {
                crash_with_message("unsupported branch invoked");
            }

            source = view_string(collection);

            if (number < 1 || (size_t) number > source->length)
            {
                return throw_error("absent key");
            }

            if (source->length > 1)
            {
                char *bytes;

                bytes = allocate(sizeof(char) * source->length);
                memcpy(bytes, source->bytes, number - 1);
                memcpy(bytes + number - 1, source->bytes + number, source->length - number);

                return steal_string(create_string(bytes, source->length - 1));
            }
            else
            {
                return steal_string(empty_string());
            }
        }

        case VALUE_TYPE_LIST:
        {
            list_t *source, *destination;
            int number;
            size_t index, cursor;

            if (!next_argument(arguments, variables, VALUE_TYPE_NUMBER, &key))
            {
                return key;
            }

            if (number_to_integer(view_number(key), &number) != 0)
            {
                crash_with_message("unsupported branch invoked");
            }

            source = view_list(collection);

            if (number < 1 || (size_t) number > source->length)
            {
                return throw_error("absent key");
            }

            index = number - 1;
            destination = empty_list(dereference_value_unsafe, 1);

            for (cursor = 0; cursor < source->length; cursor++)
            {
                if (index != cursor)
                {
                    value_t *copy;

                    copy = source->items[cursor];
                    copy->owners += 1;

                    add_list_item(destination, copy);
                }
            }

            return steal_list(destination);
        }

        case VALUE_TYPE_MAP:
        {
            map_t *source, *destination;

            if (!next_argument(arguments, variables, VALUE_TYPE_STRING, &key))
            {
                return key;
            }

            source = view_map(collection);
            destination = empty_map(hash_string, dereference_value_unsafe, 1);

            copy_map_items(source, destination);
            unset_map_item(destination, view_string(key));

            return steal_map(destination);
        }

        default:
            crash_with_message("unsupported branch invoked");
            return new_null();
    }
}

static value_t *run_merge(argument_iterator_t *arguments, map_t *variables)
{
    value_t *left, *right;

    if (!next_argument(arguments, variables, VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP, &left))
    {
        return left;
    }

    switch (left->type)
    {
        case VALUE_TYPE_STRING:
        {
            if (!next_argument(arguments, variables, VALUE_TYPE_STRING, &right))
            {
                return right;
            }

            return steal_string(merge_strings(view_string(left), view_string(right)));
        }

        case VALUE_TYPE_LIST:
        {
            list_t *source, *destination;
            size_t index;

            if (!next_argument(arguments, variables, VALUE_TYPE_LIST, &right))
            {
                return right;
            }

            destination = empty_list(dereference_value_unsafe, 1);
            source = view_list(left);

            for (index = 0; index < source->length; index++)
            {
                value_t *copy;

                copy = source->items[index];
                copy->owners += 1;

                add_list_item(destination, copy);
            }

            source = view_list(right);

            for (index = 0; index < source->length; index++)
            {
                value_t *copy;

                copy = source->items[index];
                copy->owners += 1;

                add_list_item(destination, copy);
            }

            return steal_list(destination);
        }

        case VALUE_TYPE_MAP:
        {
            map_t *source, *destination;

            if (!next_argument(arguments, variables, VALUE_TYPE_MAP, &right))
            {
                return right;
            }

            source = view_map(left);
            destination = empty_map(hash_string, dereference_value_unsafe, source->capacity);

            copy_map_items(source, destination);

            source = view_map(right);

            copy_map_items(source, destination);

            return steal_map(destination);
        }

        default:
            crash_with_message("unsupported branch invoked");
            return new_null();
    }
}

static value_t *run_length(argument_iterator_t *arguments, map_t *variables)
{
    value_t *collection;
    size_t length;
    number_t number;

    if (!next_argument(arguments, variables, VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP, &collection))
    {
        return collection;
    }

    switch (collection->type)
    {
        case VALUE_TYPE_STRING:
            length = view_string(collection)->length;
            break;

        case VALUE_TYPE_LIST:
            length = view_list(collection)->length;
            break;

        case VALUE_TYPE_MAP:
            length = view_map(collection)->length;
            break;

        default:
            crash_with_message("unsupported branch invoked");
            return new_null();
    }

    if (length >= PORTABLE_INT_LIMIT || integer_to_number(length, &number) != 0)
    {
        return throw_error("constraint error");
    }

    return new_number(number);
}

static value_t *run_keys(argument_iterator_t *arguments, map_t *variables)
{
    value_t *collection;

    if (!next_argument(arguments, variables, VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP, &collection))
    {
        return collection;
    }

    switch (collection->type)
    {
        case VALUE_TYPE_STRING:
        {
            string_t *string;
            list_t *keys;
            number_t number;
            size_t index;

            string = view_string(collection);

            if (string->length >= PORTABLE_INT_LIMIT || integer_to_number(string->length, &number) != 0)
            {
                return throw_error("constraint error");
            }

            keys = empty_list(dereference_value_unsafe, 1);

            for (index = 0; index < string->length; index++)
            {
                number_t key;

                if (index + 1 >= PORTABLE_INT_LIMIT || integer_to_number(index + 1, &key) != 0)
                {
                    destroy_list(keys);

                    return throw_error("constraint error");
                }

                add_list_item(keys, new_number(key));
            }

            return steal_list(keys);
        }

        case VALUE_TYPE_LIST:
        {
            list_t *list, *keys;
            number_t number;
            size_t length, index;

            list = view_list(collection);
            length = list->length;

            if (length >= PORTABLE_INT_LIMIT || integer_to_number(length, &number) != 0)
            {
                return throw_error("constraint error");
            }

            keys = empty_list(dereference_value_unsafe, 1);

            for (index = 0; index < length; index++)
            {
                number_t key;

                if (index + 1 >= PORTABLE_INT_LIMIT || integer_to_number(index + 1, &key) != 0)
                {
                    destroy_list(keys);

                    return throw_error("constraint error");
                }

                add_list_item(keys, new_number(key));
            }

            return steal_list(keys);
        }

        case VALUE_TYPE_MAP:
        {
            map_t *map;
            list_t *keys;
            string_t **rawKeys;
            size_t index;

            map = view_map(collection);
            keys = empty_list(dereference_value_unsafe, 1);
            rawKeys = list_map_keys(map);

            for (index = 0; index < map->length; index++)
            {
                add_list_item(keys, steal_string(copy_string(rawKeys[index])));
            }

            free(rawKeys);

            return steal_list(keys);
        }

        default:
            crash_with_message("unsupported branch invoked");
            return new_null();
    }
}

static value_t *run_sort(argument_iterator_t *arguments, map_t *variables)
{
    value_t *collection, *direction;
    list_t *source, *destination;
    int ascending;
    size_t index;

    if (!next_argument(arguments, variables, VALUE_TYPE_LIST, &collection))
    {
        return collection;
    }

    if (!next_argument(arguments, variables, VALUE_TYPE_STRING, &direction))
    {
        return direction;
    }

    if (is_keyword_match(view_string(direction), "+"))
    {
        ascending = 1;
    }
    else if (is_keyword_match(view_string(direction), "-"))
    {
        ascending = 0;
    }
    else
    {
        return throw_error("invalid direction");
    }

    source = view_list(collection);
    destination = empty_list(dereference_value_unsafe, 1);

    for (index = 0; index < source->length; index++)
    {
        value_t *copy;

        copy = source->items[index];
        copy->owners += 1;

        add_list_item(destination, copy);
    }

    qsort(destination->items, destination->length, sizeof(value_t *), ascending ? compare_values_ascending : compare_values_descending);

    return steal_list(destination);
}

static int next_argument(argument_iterator_t *arguments, map_t *variables, int types, value_t **out)
{
    value_t *result;

    if (!has_next_argument(arguments))
    {
        (*out) = throw_error("absent argument");
        return 0;
    }

    result = apply_statement(arguments->statements->items[arguments->index], variables);
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

static void copy_map_items(map_t *source, map_t *destination)
{
    size_t index;

    for (index = 0; index < source->capacity; index++)
    {
        if (source->chains[index])
        {
            map_chain_t *chain;

            for (chain = source->chains[index]; chain != NULL; chain = chain->next)
            {
                value_t *copy;

                copy = get_map_item(source, chain->key);
                copy->owners += 1;

                set_map_item(destination, copy_string(chain->key), copy);
            }
        }
    }
}

static int compare_values_ascending(const void *left, const void *right)
{
    return compare_values(*(value_t **) left, *(value_t **) right);
}

static int compare_values_descending(const void *left, const void *right)
{
    return compare_values(*(value_t **) left, *(value_t **) right) * -1;
}

static void dereference_value_unsafe(void *value)
{
    dereference_value(value);
}
