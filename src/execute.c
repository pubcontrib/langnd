#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "execute.h"
#include "parse.h"
#include "utility.h"

typedef struct
{
    list_t *expressions;
    value_t **evaluated;
    size_t index;
} argument_iterator_t;

typedef enum
{
    VALUE_EFFECT_PROGRESS,
    VALUE_EFFECT_RETURN,
    VALUE_EFFECT_BREAK,
    VALUE_EFFECT_CONTINUE,
    VALUE_EFFECT_THROW
} value_effect_t;

typedef struct invoke_frame_t
{
    map_t *variables;
    argument_iterator_t arguments;
    value_effect_t effect;
    struct invoke_frame_t *parent;
} invoke_frame_t;

static value_t *apply_expression(expression_t *expression, invoke_frame_t *frame);
static value_t *run_add(invoke_frame_t *frame);
static value_t *run_subtract(invoke_frame_t *frame);
static value_t *run_multiply(invoke_frame_t *frame);
static value_t *run_divide(invoke_frame_t *frame);
static value_t *run_modulo(invoke_frame_t *frame);
static value_t *run_truncate(invoke_frame_t *frame);
static value_t *run_and(invoke_frame_t *frame);
static value_t *run_or(invoke_frame_t *frame);
static value_t *run_not(invoke_frame_t *frame);
static value_t *run_precedes(invoke_frame_t *frame);
static value_t *run_succeeds(invoke_frame_t *frame);
static value_t *run_equals(invoke_frame_t *frame);
static value_t *run_write(invoke_frame_t *frame);
static value_t *run_read(invoke_frame_t *frame);
static value_t *run_delete(invoke_frame_t *frame);
static value_t *run_query(invoke_frame_t *frame);
static value_t *run_freeze(invoke_frame_t *frame);
static value_t *run_thaw(invoke_frame_t *frame);
static value_t *run_type(invoke_frame_t *frame);
static value_t *run_cast(invoke_frame_t *frame);
static value_t *run_get(invoke_frame_t *frame);
static value_t *run_set(invoke_frame_t *frame);
static value_t *run_unset(invoke_frame_t *frame);
static value_t *run_merge(invoke_frame_t *frame);
static value_t *run_length(invoke_frame_t *frame);
static value_t *run_keys(invoke_frame_t *frame);
static value_t *run_sort(invoke_frame_t *frame);
static int next_argument(int types, value_t **out, invoke_frame_t *frame);
static int has_next_argument(const invoke_frame_t *frame);
static void copy_map_items(const map_t *source, map_t *destination);
static int compare_values_ascending(const void *left, const void *right);
static int compare_values_descending(const void *left, const void *right);
static value_t *throw_error(const char *message, invoke_frame_t *frame);
static void dereference_value_unsafe(void *value);

outcome_t *execute(string_t *code)
{
    outcome_t *outcome;
    script_t *script;
    invoke_frame_t root;
    list_t *expressions;
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

    root.variables = empty_map(hash_string, dereference_value_unsafe, 8);
    root.effect = VALUE_EFFECT_PROGRESS;
    expressions = script->expressions;

    for (index = 0; index < expressions->length; index++)
    {
        value_t *result;

        result = apply_expression(expressions->items[index], &root);

        if (root.effect == VALUE_EFFECT_PROGRESS)
        {
            dereference_value(result);
        }
        else if (root.effect == VALUE_EFFECT_RETURN)
        {
            dereference_value(result);
            result = steal_string(cstring_to_string("lost return"));
            outcome->errorMessage = cstring_to_string("failed to execute code");
            outcome->hintMessage = represent_value(result);
            dereference_value(result);

            break;
        }
        else if (root.effect == VALUE_EFFECT_BREAK)
        {
            dereference_value(result);
            result = steal_string(cstring_to_string("lost break"));
            outcome->errorMessage = cstring_to_string("failed to execute code");
            outcome->hintMessage = represent_value(result);
            dereference_value(result);

            break;
        }
        else if (root.effect == VALUE_EFFECT_CONTINUE)
        {
            dereference_value(result);
            result = steal_string(cstring_to_string("lost continue"));
            outcome->errorMessage = cstring_to_string("failed to execute code");
            outcome->hintMessage = represent_value(result);
            dereference_value(result);

            break;
        }
        else if (root.effect == VALUE_EFFECT_THROW)
        {
            outcome->errorMessage = cstring_to_string("failed to execute code");
            outcome->hintMessage = represent_value(result);
            dereference_value(result);

            break;
        }
    }

    destroy_map(root.variables);
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

static value_t *apply_expression(expression_t *expression, invoke_frame_t *frame)
{
    switch (expression->type)
    {
        case EXPRESSION_TYPE_LITERAL:
        {
            literal_expression_data_t *data;
            value_t *value;

            data = expression->data;
            value = data->value;
            value->owners += 1;

            return value;
        }

        case EXPRESSION_TYPE_REFERENCE:
        {
            reference_expression_data_t *data;

            data = expression->data;

            if (data->identifier->type == IDENTIFIER_TYPE_VARIABLE)
            {
                value_t *value;

                value = get_map_item(frame->variables, data->identifier->name);

                if (value)
                {
                    value->owners += 1;
                    return value;
                }
                else
                {
                    return throw_error("absent variable", frame);
                }
            }
            else if (data->identifier->type == IDENTIFIER_TYPE_FUNCTION)
            {
                return throw_error("unexpected reference type", frame);
            }
            else
            {
                crash_with_message("unsupported branch invoked");
                return new_null();
            }
        }

        case EXPRESSION_TYPE_ASSIGNMENT:
        {
            assignment_expression_data_t *data;
            value_t *value;

            data = expression->data;
            value = apply_expression(data->value, frame);

            if (frame->effect == VALUE_EFFECT_RETURN || frame->effect == VALUE_EFFECT_BREAK || frame->effect == VALUE_EFFECT_CONTINUE || frame->effect == VALUE_EFFECT_THROW)
            {
                return value;
            }

            set_map_item(frame->variables, copy_string(data->identifier->name), value);
            value->owners += 1;

            return value;
        }

        case EXPRESSION_TYPE_INVOKE:
        {
            invoke_expression_data_t *data;
            invoke_frame_t descendant;
            value_t *result;

            data = expression->data;
            descendant.variables = empty_map(hash_string, dereference_value_unsafe, 8);
            descendant.arguments.expressions = data->arguments;

            if (data->arguments->length > 0)
            {
                descendant.arguments.evaluated = allocate_with_zeros(sizeof(value_t *), data->arguments->length);
            }
            else
            {
                descendant.arguments.evaluated = NULL;
            }

            descendant.arguments.index = 0;
            descendant.effect = VALUE_EFFECT_PROGRESS;
            descendant.parent = frame;

            if (data->identifier->type == IDENTIFIER_TYPE_FUNCTION)
            {
                if (is_keyword_match(data->identifier->name, "add"))
                {
                    result = run_add(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "subtract"))
                {
                    result = run_subtract(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "multiply"))
                {
                    result = run_multiply(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "divide"))
                {
                    result = run_divide(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "modulo"))
                {
                    result = run_modulo(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "truncate"))
                {
                    result = run_truncate(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "and"))
                {
                    result = run_and(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "or"))
                {
                    result = run_or(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "not"))
                {
                    result = run_not(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "precedes"))
                {
                    result = run_precedes(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "succeeds"))
                {
                    result = run_succeeds(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "equals"))
                {
                    result = run_equals(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "write"))
                {
                    result = run_write(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "read"))
                {
                    result = run_read(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "delete"))
                {
                    result = run_delete(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "query"))
                {
                    result = run_query(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "freeze"))
                {
                    result = run_freeze(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "thaw"))
                {
                    result = run_thaw(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "type"))
                {
                    result = run_type(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "cast"))
                {
                    result = run_cast(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "get"))
                {
                    result = run_get(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "set"))
                {
                    result = run_set(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "unset"))
                {
                    result = run_unset(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "merge"))
                {
                    result = run_merge(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "length"))
                {
                    result = run_length(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "keys"))
                {
                    result = run_keys(&descendant);
                }
                else if (is_keyword_match(data->identifier->name, "sort"))
                {
                    result = run_sort(&descendant);
                }
                else
                {
                    result = throw_error("absent function", &descendant);
                }
            }
            else if (data->identifier->type == IDENTIFIER_TYPE_VARIABLE)
            {
                value_t *value;

                value = get_map_item(frame->variables, data->identifier->name);

                if (value)
                {
                    if (value->type == VALUE_TYPE_FUNCTION)
                    {
                        function_t *function;
                        list_t *expressions;
                        value_t *last;
                        size_t index;

                        function = view_function(value);
                        expressions = function->expressions;
                        last = NULL;

                        for (index = 0; index < expressions->length; index++)
                        {
                            if (last)
                            {
                                dereference_value(last);
                            }

                            last = apply_expression(expressions->items[index], &descendant);

                            if (descendant.effect == VALUE_EFFECT_RETURN)
                            {
                                descendant.effect = VALUE_EFFECT_PROGRESS;
                                break;
                            }
                            else if (descendant.effect == VALUE_EFFECT_BREAK || descendant.effect == VALUE_EFFECT_CONTINUE || descendant.effect == VALUE_EFFECT_THROW)
                            {
                                break;
                            }
                        }

                        if (!last)
                        {
                            last = new_null();
                        }

                        result = last;
                    }
                    else
                    {
                        result = throw_error("invocation error", &descendant);
                    }
                }
                else
                {
                    result = throw_error("absent variable", &descendant);
                }
            }
            else
            {
                crash_with_message("unsupported branch invoked");
                return new_null();
            }

            destroy_map(descendant.variables);

            if (descendant.arguments.evaluated)
            {
                size_t index;

                for (index = 0; index < descendant.arguments.index; index++)
                {
                    dereference_value(descendant.arguments.evaluated[index]);
                }

                free(descendant.arguments.evaluated);
            }

            frame->effect = descendant.effect;

            return result;
        }

        case EXPRESSION_TYPE_BRANCH:
        {
            branch_expression_data_t *data;
            list_t *branches;
            value_t *last;
            size_t index;

            data = expression->data;
            branches = data->branches;
            last = NULL;

            for (index = 0; index < branches->length; index++)
            {
                conditional_branch_t *branch;
                value_t *test;

                branch = branches->items[index];
                test = apply_expression(branch->condition, frame);

                if (frame->effect == VALUE_EFFECT_RETURN || frame->effect == VALUE_EFFECT_BREAK || frame->effect == VALUE_EFFECT_CONTINUE || frame->effect == VALUE_EFFECT_THROW)
                {
                    return test;
                }

                if (test->type == VALUE_TYPE_BOOLEAN)
                {
                    boolean_t pass;

                    pass = view_boolean(test);
                    dereference_value(test);

                    if (pass == BOOLEAN_TRUE)
                    {
                        last = apply_expression(branch->action, frame);

                        break;
                    }
                }
                else
                {
                    dereference_value(test);

                    return throw_error("branch with non-boolean condition", frame);
                }
            }

            if (!last)
            {
                last = new_null();
            }

            return last;
        }

        case EXPRESSION_TYPE_LOOP:
        {
            loop_expression_data_t *data;
            value_t *last;

            data = expression->data;
            last = NULL;

            while (1)
            {
                value_t *test;

                test = apply_expression(data->condition, frame);

                if (frame->effect == VALUE_EFFECT_RETURN || frame->effect == VALUE_EFFECT_BREAK || frame->effect == VALUE_EFFECT_CONTINUE || frame->effect == VALUE_EFFECT_THROW)
                {
                    return test;
                }

                if (test->type == VALUE_TYPE_BOOLEAN)
                {
                    if (view_boolean(test) == BOOLEAN_FALSE)
                    {
                        dereference_value(test);
                        break;
                    }

                    dereference_value(test);

                    if (last)
                    {
                        dereference_value(last);
                    }

                    last = apply_expression(data->action, frame);

                    if (frame->effect == VALUE_EFFECT_BREAK)
                    {
                        frame->effect = VALUE_EFFECT_PROGRESS;
                        break;
                    }
                    else if (frame->effect == VALUE_EFFECT_CONTINUE)
                    {
                        frame->effect = VALUE_EFFECT_PROGRESS;
                        continue;
                    }
                    else if (frame->effect == VALUE_EFFECT_THROW)
                    {
                        break;
                    }
                }
                else
                {
                    dereference_value(test);

                    return throw_error("loop with non-boolean condition", frame);
                }
            }

            if (!last)
            {
                last = new_null();
            }

            return last;
        }

        case EXPRESSION_TYPE_CATCH:
        {
            catch_expression_data_t *data;
            value_t *last;

            data = expression->data;
            last = apply_expression(data->action, frame);

            if (frame->effect == VALUE_EFFECT_RETURN || frame->effect == VALUE_EFFECT_BREAK || frame->effect == VALUE_EFFECT_CONTINUE)
            {
                return last;
            }
            else if (frame->effect == VALUE_EFFECT_THROW)
            {
                frame->effect = VALUE_EFFECT_PROGRESS;

                return last;
            }
            else
            {
                dereference_value(last);
            }

            return new_null();
        }

        case EXPRESSION_TYPE_ARGUMENT:
        {
            value_t *argument;

            if (next_argument(VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP | VALUE_TYPE_FUNCTION, &argument, frame))
            {
                argument->owners += 1;
            }

            return argument;
        }

        case EXPRESSION_TYPE_RETURN:
        {
            return_expression_data_t *data;
            value_t *test;

            data = expression->data;
            test = apply_expression(data->pick, frame);

            if (frame->effect == VALUE_EFFECT_RETURN || frame->effect == VALUE_EFFECT_BREAK || frame->effect == VALUE_EFFECT_CONTINUE || frame->effect == VALUE_EFFECT_THROW)
            {
                return test;
            }

            frame->effect = VALUE_EFFECT_RETURN;

            return test;
        }

        case EXPRESSION_TYPE_BREAK:
        {
            break_expression_data_t *data;
            value_t *test;

            data = expression->data;
            test = apply_expression(data->pick, frame);

            if (frame->effect == VALUE_EFFECT_RETURN || frame->effect == VALUE_EFFECT_BREAK || frame->effect == VALUE_EFFECT_CONTINUE || frame->effect == VALUE_EFFECT_THROW)
            {
                return test;
            }

            frame->effect = VALUE_EFFECT_BREAK;

            return test;
        }

        case EXPRESSION_TYPE_CONTINUE:
        {
            continue_expression_data_t *data;
            value_t *test;

            data = expression->data;
            test = apply_expression(data->pick, frame);

            if (frame->effect == VALUE_EFFECT_RETURN || frame->effect == VALUE_EFFECT_BREAK || frame->effect == VALUE_EFFECT_CONTINUE || frame->effect == VALUE_EFFECT_THROW)
            {
                return test;
            }

            frame->effect = VALUE_EFFECT_CONTINUE;

            return test;
        }

        case EXPRESSION_TYPE_THROW:
        {
            throw_expression_data_t *data;
            value_t *test;

            data = expression->data;
            test = apply_expression(data->error, frame);

            if (frame->effect == VALUE_EFFECT_RETURN || frame->effect == VALUE_EFFECT_BREAK || frame->effect == VALUE_EFFECT_CONTINUE || frame->effect == VALUE_EFFECT_THROW)
            {
                return test;
            }

            frame->effect = VALUE_EFFECT_THROW;

            return test;
        }

        case EXPRESSION_TYPE_SNIPPET:
        {
            snippet_expression_data_t *data;
            list_t *expressions;
            value_t *last;
            size_t index;

            data = expression->data;
            expressions = data->expressions;
            last = NULL;

            for (index = 0; index < expressions->length; index++)
            {
                if (last)
                {
                    dereference_value(last);
                }

                last = apply_expression(expressions->items[index], frame);

                if (frame->effect == VALUE_EFFECT_RETURN || frame->effect == VALUE_EFFECT_BREAK || frame->effect == VALUE_EFFECT_CONTINUE || frame->effect == VALUE_EFFECT_THROW)
                {
                    break;
                }
            }

            if (!last)
            {
                last = new_null();
            }

            return last;
        }

        default:
            break;
    }

    return new_null();
}

static value_t *run_add(invoke_frame_t *frame)
{
    value_t *left, *right;
    number_t sum;

    if (!next_argument(VALUE_TYPE_NUMBER, &left, frame))
    {
        return left;
    }

    if (!next_argument(VALUE_TYPE_NUMBER, &right, frame))
    {
        return right;
    }

    if (add_numbers(view_number(left), view_number(right), &sum) != 0)
    {
        return throw_error("arithmetic error", frame);
    }

    return new_number(sum);
}

static value_t *run_subtract(invoke_frame_t *frame)
{
    value_t *left, *right;
    number_t difference;

    if (!next_argument(VALUE_TYPE_NUMBER, &left, frame))
    {
        return left;
    }

    if (!next_argument(VALUE_TYPE_NUMBER, &right, frame))
    {
        return right;
    }

    if (subtract_numbers(view_number(left), view_number(right), &difference) != 0)
    {
        return throw_error("arithmetic error", frame);
    }

    return new_number(difference);
}

static value_t *run_multiply(invoke_frame_t *frame)
{
    value_t *left, *right;
    number_t product;

    if (!next_argument(VALUE_TYPE_NUMBER, &left, frame))
    {
        return left;
    }

    if (!next_argument(VALUE_TYPE_NUMBER, &right, frame))
    {
        return right;
    }

    if (multiply_numbers(view_number(left), view_number(right), &product) != 0)
    {
        return throw_error("arithmetic error", frame);
    }

    return new_number(product);
}

static value_t *run_divide(invoke_frame_t *frame)
{
    value_t *left, *right;
    number_t quotient;

    if (!next_argument(VALUE_TYPE_NUMBER, &left, frame))
    {
        return left;
    }

    if (!next_argument(VALUE_TYPE_NUMBER, &right, frame))
    {
        return right;
    }

    if (divide_numbers(view_number(left), view_number(right), &quotient) != 0)
    {
        return throw_error("arithmetic error", frame);
    }

    return new_number(quotient);
}

static value_t *run_modulo(invoke_frame_t *frame)
{
    value_t *left, *right;
    number_t remainder;

    if (!next_argument(VALUE_TYPE_NUMBER, &left, frame))
    {
        return left;
    }

    if (!next_argument(VALUE_TYPE_NUMBER, &right, frame))
    {
        return right;
    }

    if (modulo_numbers(view_number(left), view_number(right), &remainder) != 0)
    {
        return throw_error("arithmetic error", frame);
    }

    return new_number(remainder);
}

static value_t *run_truncate(invoke_frame_t *frame)
{
    value_t *value;

    if (!next_argument(VALUE_TYPE_NUMBER, &value, frame))
    {
        return value;
    }

    return new_number(truncate_number(view_number(value)));
}

static value_t *run_and(invoke_frame_t *frame)
{
    value_t *left, *right;

    if (!next_argument(VALUE_TYPE_BOOLEAN, &left, frame))
    {
        return left;
    }

    if (!next_argument(VALUE_TYPE_BOOLEAN, &right, frame))
    {
        return right;
    }

    return new_boolean(view_boolean(left) && view_boolean(right) ? BOOLEAN_TRUE : BOOLEAN_FALSE);
}

static value_t *run_or(invoke_frame_t *frame)
{
    value_t *left, *right;

    if (!next_argument(VALUE_TYPE_BOOLEAN, &left, frame))
    {
        return left;
    }

    if (!next_argument(VALUE_TYPE_BOOLEAN, &right, frame))
    {
        return right;
    }

    return new_boolean(view_boolean(left) || view_boolean(right) ? BOOLEAN_TRUE : BOOLEAN_FALSE);
}

static value_t *run_not(invoke_frame_t *frame)
{
    value_t *value;

    if (!next_argument(VALUE_TYPE_BOOLEAN, &value, frame))
    {
        return value;
    }

    return new_boolean(!view_boolean(value) ? BOOLEAN_TRUE : BOOLEAN_FALSE);
}

static value_t *run_precedes(invoke_frame_t *frame)
{
    value_t *left, *right;

    if (!next_argument(VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP | VALUE_TYPE_FUNCTION, &left, frame))
    {
        return left;
    }

    if (!next_argument(VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP | VALUE_TYPE_FUNCTION, &right, frame))
    {
        return right;
    }

    return new_boolean(compare_values(left, right) < 0 ? BOOLEAN_TRUE : BOOLEAN_FALSE);
}

static value_t *run_succeeds(invoke_frame_t *frame)
{
    value_t *left, *right;

    if (!next_argument(VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP | VALUE_TYPE_FUNCTION, &left, frame))
    {
        return left;
    }

    if (!next_argument(VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP | VALUE_TYPE_FUNCTION, &right, frame))
    {
        return right;
    }

    return new_boolean(compare_values(left, right) > 0 ? BOOLEAN_TRUE : BOOLEAN_FALSE);
}

static value_t *run_equals(invoke_frame_t *frame)
{
    value_t *left, *right;

    if (!next_argument(VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP | VALUE_TYPE_FUNCTION, &left, frame))
    {
        return left;
    }

    if (!next_argument(VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP | VALUE_TYPE_FUNCTION, &right, frame))
    {
        return right;
    }

    return new_boolean(compare_values(left, right) == 0 ? BOOLEAN_TRUE : BOOLEAN_FALSE);
}

static value_t *run_write(invoke_frame_t *frame)
{
    value_t *message, *file;
    FILE *handle;
    int closable, flushable;
    string_t *string;

    if (!next_argument(VALUE_TYPE_STRING, &message, frame))
    {
        return message;
    }

    if (!next_argument(VALUE_TYPE_NUMBER | VALUE_TYPE_STRING, &file, frame))
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
        return throw_error("absent file", frame);
    }

    fwrite(string->bytes, sizeof(char), string->length, handle);

    if (ferror(handle))
    {
        if (closable)
        {
            fclose(handle);
        }

        return throw_error("io error", frame);
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

static value_t *run_read(invoke_frame_t *frame)
{
    value_t *file, *until;
    FILE *handle;
    int closable, terminated;
    string_t *terminator;
    char *bytes;
    size_t fill, length;

    if (!next_argument(VALUE_TYPE_NUMBER | VALUE_TYPE_STRING, &file, frame))
    {
        return file;
    }

    if (!next_argument(VALUE_TYPE_NULL | VALUE_TYPE_STRING, &until, frame))
    {
        return until;
    }

    terminated = until->type == VALUE_TYPE_STRING;
    terminator = terminated ? view_string(until) : NULL;

    if (terminated && terminator->length != 1)
    {
        return throw_error("invalid terminator", frame);
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
        return throw_error("absent file", frame);
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

            return throw_error("io error", frame);
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

static value_t *run_delete(invoke_frame_t *frame)
{
    value_t *file;

    if (!next_argument(VALUE_TYPE_NUMBER | VALUE_TYPE_STRING, &file, frame))
    {
        return file;
    }

    switch (file->type)
    {
        case VALUE_TYPE_NUMBER:
        {
            return throw_error("io error", frame);
        }

        case VALUE_TYPE_STRING:
        {
            char *cstring;
            int status;

            cstring = string_to_cstring(view_string(file));
            status = remove(cstring);
            free(cstring);

            if (status != -1)
            {
                return new_null();
            }
            else
            {
                return throw_error("io error", frame);
            }
        }

        default:
            crash_with_message("unsupported branch invoked");
            return new_null();
    }
}

static value_t *run_query(invoke_frame_t *frame)
{
    value_t *key;
    char *value, *cstring;

    if (!next_argument(VALUE_TYPE_STRING, &key, frame))
    {
        return key;
    }

    cstring = string_to_cstring(view_string(key));
    value = getenv(cstring);
    free(cstring);

    if (value)
    {
        return steal_string(cstring_to_string(value));
    }
    else
    {
        return throw_error("absent environment variable", frame);
    }
}

static value_t *run_freeze(invoke_frame_t *frame)
{
    value_t *value;

    if (!next_argument(VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP | VALUE_TYPE_FUNCTION, &value, frame))
    {
        return value;
    }

    return steal_string(represent_value(value));
}

static value_t *run_thaw(invoke_frame_t *frame)
{
    value_t *code, *value;
    script_t *script;
    expression_t *expression;
    literal_expression_data_t *data;

    if (!next_argument(VALUE_TYPE_STRING, &code, frame))
    {
        return code;
    }

    script = parse_script(view_string(code));

    if (script->errorMessage || script->expressions->length != 1)
    {
        destroy_script(script);

        return throw_error("melting error", frame);
    }

    expression = script->expressions->items[0];

    if (expression->type != EXPRESSION_TYPE_LITERAL)
    {
        destroy_script(script);

        return throw_error("melting error", frame);
    }

    data = expression->data;
    value = data->value;
    data->value = NULL;
    destroy_script(script);

    return value;
}

static value_t *run_type(invoke_frame_t *frame)
{
    value_t *value;

    if (!next_argument(VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP | VALUE_TYPE_FUNCTION, &value, frame))
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

        case VALUE_TYPE_FUNCTION:
            return steal_string(cstring_to_string("FUNCTION"));

        default:
            crash_with_message("unsupported branch invoked");
            return new_null();
    }
}

static value_t *run_cast(invoke_frame_t *frame)
{
    value_t *value, *type;
    string_t *pattern;

    if (!next_argument(VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP | VALUE_TYPE_FUNCTION, &value, frame))
    {
        return value;
    }

    if (!next_argument(VALUE_TYPE_STRING, &type, frame))
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
                return throw_error("invalid cast", frame);
            }
        }
        else
        {
            return throw_error("invalid cast", frame);
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
                return new_boolean(BOOLEAN_FALSE);
            }
            else if (is_keyword_match(string, "true"))
            {
                return new_boolean(BOOLEAN_TRUE);
            }
            else
            {
                return throw_error("invalid cast", frame);
            }
        }
        else
        {
            return throw_error("invalid cast", frame);
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
                return throw_error("invalid cast", frame);
            }

            return new_number(number);
        }
        else
        {
            return throw_error("invalid cast", frame);
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
            return view_boolean(value) == BOOLEAN_TRUE ? steal_string(cstring_to_string("true")) : steal_string(cstring_to_string("false"));
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
            return throw_error("invalid cast", frame);
        }
    }
    else if (is_keyword_match(pattern, "LIST"))
    {
        if (value->type == VALUE_TYPE_LIST)
        {
            value->owners += 1;

            return value;
        }
        else
        {
            return throw_error("invalid cast", frame);
        }
    }
    else if (is_keyword_match(pattern, "MAP"))
    {
        if (value->type == VALUE_TYPE_MAP)
        {
            value->owners += 1;

            return value;
        }
        else
        {
            return throw_error("invalid cast", frame);
        }
    }
    else if (is_keyword_match(pattern, "FUNCTION"))
    {
        if (value->type == VALUE_TYPE_FUNCTION)
        {
            value->owners += 1;

            return value;
        }
        else
        {
            return throw_error("invalid cast", frame);
        }
    }
    else
    {
        return throw_error("unknown type", frame);
    }
}

static value_t *run_get(invoke_frame_t *frame)
{
    value_t *collection, *key;

    if (!next_argument(VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP, &collection, frame))
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

            if (!next_argument(VALUE_TYPE_NUMBER, &key, frame))
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
                return throw_error("absent key", frame);
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

            if (!next_argument(VALUE_TYPE_NUMBER, &key, frame))
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
                return throw_error("absent key", frame);
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

            if (!next_argument(VALUE_TYPE_STRING, &key, frame))
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
                return throw_error("absent key", frame);
            }
        }

        default:
            crash_with_message("unsupported branch invoked");
            return new_null();
    }
}

static value_t *run_set(invoke_frame_t *frame)
{
    value_t *collection, *key, *item;

    if (!next_argument(VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP, &collection, frame))
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

            if (!next_argument(VALUE_TYPE_NUMBER, &key, frame))
            {
                return key;
            }

            if (!next_argument(VALUE_TYPE_STRING, &item, frame))
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
                return throw_error("absent key", frame);
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

            if (!next_argument(VALUE_TYPE_NUMBER, &key, frame))
            {
                return key;
            }

            if (!next_argument(VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP, &item, frame))
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
                return throw_error("absent key", frame);
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

            if (!next_argument(VALUE_TYPE_STRING, &key, frame))
            {
                return key;
            }

            if (!next_argument(VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP, &item, frame))
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

static value_t *run_unset(invoke_frame_t *frame)
{
    value_t *collection, *key;

    if (!next_argument(VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP, &collection, frame))
    {
        return collection;
    }

    switch (collection->type)
    {
        case VALUE_TYPE_STRING:
        {
            string_t *source;
            int number;

            if (!next_argument(VALUE_TYPE_NUMBER, &key, frame))
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
                return throw_error("absent key", frame);
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

            if (!next_argument(VALUE_TYPE_NUMBER, &key, frame))
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
                return throw_error("absent key", frame);
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

            if (!next_argument(VALUE_TYPE_STRING, &key, frame))
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

static value_t *run_merge(invoke_frame_t *frame)
{
    value_t *left, *right;

    if (!next_argument(VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP, &left, frame))
    {
        return left;
    }

    switch (left->type)
    {
        case VALUE_TYPE_STRING:
        {
            if (!next_argument(VALUE_TYPE_STRING, &right, frame))
            {
                return right;
            }

            return steal_string(merge_strings(view_string(left), view_string(right)));
        }

        case VALUE_TYPE_LIST:
        {
            list_t *source, *destination;
            size_t index;

            if (!next_argument(VALUE_TYPE_LIST, &right, frame))
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

            if (!next_argument(VALUE_TYPE_MAP, &right, frame))
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

static value_t *run_length(invoke_frame_t *frame)
{
    value_t *collection;
    size_t length;
    number_t number;

    if (!next_argument(VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP, &collection, frame))
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
        return throw_error("constraint error", frame);
    }

    return new_number(number);
}

static value_t *run_keys(invoke_frame_t *frame)
{
    value_t *collection;

    if (!next_argument(VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP, &collection, frame))
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
                return throw_error("constraint error", frame);
            }

            keys = empty_list(dereference_value_unsafe, 1);

            for (index = 0; index < string->length; index++)
            {
                number_t key;

                if (index + 1 >= PORTABLE_INT_LIMIT || integer_to_number(index + 1, &key) != 0)
                {
                    destroy_list(keys);

                    return throw_error("constraint error", frame);
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
                return throw_error("constraint error", frame);
            }

            keys = empty_list(dereference_value_unsafe, 1);

            for (index = 0; index < length; index++)
            {
                number_t key;

                if (index + 1 >= PORTABLE_INT_LIMIT || integer_to_number(index + 1, &key) != 0)
                {
                    destroy_list(keys);

                    return throw_error("constraint error", frame);
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

static value_t *run_sort(invoke_frame_t *frame)
{
    value_t *collection, *direction;
    list_t *source, *destination;
    int ascending;
    size_t index;

    if (!next_argument(VALUE_TYPE_LIST, &collection, frame))
    {
        return collection;
    }

    if (!next_argument(VALUE_TYPE_STRING, &direction, frame))
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
        return throw_error("invalid direction", frame);
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

static int next_argument(int types, value_t **out, invoke_frame_t *frame)
{
    value_t *result;

    if (!has_next_argument(frame))
    {
        (*out) = throw_error("absent argument", frame);
        return 0;
    }

    result = apply_expression(frame->arguments.expressions->items[frame->arguments.index], frame->parent);
    frame->arguments.evaluated[frame->arguments.index] = result;
    frame->arguments.index += 1;
    frame->effect = frame->parent->effect;
    frame->parent->effect = VALUE_EFFECT_PROGRESS;

    if (frame->effect == VALUE_EFFECT_RETURN || frame->effect == VALUE_EFFECT_BREAK || frame->effect == VALUE_EFFECT_CONTINUE || frame->effect == VALUE_EFFECT_THROW)
    {
        result->owners += 1;
        (*out) = result;
        return 0;
    }

    if (!(types & result->type))
    {
        (*out) = throw_error("wrong argument type", frame);
        return 0;
    }

    (*out) = result;
    return 1;
}

static int has_next_argument(const invoke_frame_t *frame)
{
    return frame->arguments.index < frame->arguments.expressions->length;
}

static void copy_map_items(const map_t *source, map_t *destination)
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

static value_t *throw_error(const char *message, invoke_frame_t *frame)
{
    frame->effect = VALUE_EFFECT_THROW;
    return steal_string(cstring_to_string(message));
}

static void dereference_value_unsafe(void *value)
{
    dereference_value(value);
}
