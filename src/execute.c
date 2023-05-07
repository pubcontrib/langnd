#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "execute.h"
#include "parse.h"
#include "utility.h"

#define VALUE_TYPES_ANY (VALUE_TYPE_NULL | VALUE_TYPE_BOOLEAN | VALUE_TYPE_NUMBER | VALUE_TYPES_COLLECTION | VALUE_TYPE_FUNCTION)
#define VALUE_TYPES_COLLECTION (VALUE_TYPE_STRING | VALUE_TYPE_LIST | VALUE_TYPE_MAP)

typedef struct
{
    list_t *expressions;
    value_t **evaluated;
    size_t index;
} argument_iterator_t;

typedef struct frame_t
{
    map_t *variables;
    argument_iterator_t arguments;
    struct frame_t *parent;
} frame_t;

typedef struct
{
    value_t *(*run)(frame_t *, machine_t *);
} native_t;

static value_t *apply_expression(expression_t *expression, frame_t *frame, machine_t *machine);
static value_t *run_add(frame_t *frame, machine_t *machine);
static value_t *run_subtract(frame_t *frame, machine_t *machine);
static value_t *run_multiply(frame_t *frame, machine_t *machine);
static value_t *run_divide(frame_t *frame, machine_t *machine);
static value_t *run_modulo(frame_t *frame, machine_t *machine);
static value_t *run_truncate(frame_t *frame, machine_t *machine);
static value_t *run_and(frame_t *frame, machine_t *machine);
static value_t *run_or(frame_t *frame, machine_t *machine);
static value_t *run_not(frame_t *frame, machine_t *machine);
static value_t *run_precedes(frame_t *frame, machine_t *machine);
static value_t *run_succeeds(frame_t *frame, machine_t *machine);
static value_t *run_equals(frame_t *frame, machine_t *machine);
static value_t *run_write(frame_t *frame, machine_t *machine);
static value_t *run_read(frame_t *frame, machine_t *machine);
static value_t *run_delete(frame_t *frame, machine_t *machine);
static value_t *run_query(frame_t *frame, machine_t *machine);
static value_t *run_evaluate(frame_t *frame, machine_t *machine);
static value_t *run_freeze(frame_t *frame, machine_t *machine);
static value_t *run_thaw(frame_t *frame, machine_t *machine);
static value_t *run_type(frame_t *frame, machine_t *machine);
static value_t *run_cast(frame_t *frame, machine_t *machine);
static value_t *run_get(frame_t *frame, machine_t *machine);
static value_t *run_set(frame_t *frame, machine_t *machine);
static value_t *run_unset(frame_t *frame, machine_t *machine);
static value_t *run_merge(frame_t *frame, machine_t *machine);
static value_t *run_length(frame_t *frame, machine_t *machine);
static value_t *run_keys(frame_t *frame, machine_t *machine);
static value_t *run_sort(frame_t *frame, machine_t *machine);
static map_t *create_machine_natives();
static map_t *create_machine_elements(const map_t *natives, int argc, char **argv, int skip);
static value_t *create_element_function(string_t *name);
static native_t *create_native(value_t *(*run)(frame_t *, machine_t *));
static int next_argument(int types, value_t **out, frame_t *frame, machine_t *machine);
static int has_next_argument(const frame_t *frame);
static void copy_map_items(const map_t *source, map_t *destination);
static int compare_values_ascending(const void *left, const void *right);
static int compare_values_descending(const void *left, const void *right);
static value_t *throw_error(const char *message, machine_t *machine);
static void destroy_expression_unsafe(void *expression);
static void destroy_native_unsafe(void *native);
static void dereference_value_unsafe(void *value);
static int has_halting_effect(const machine_t *machine);

machine_t *empty_machine(int argc, char **argv, int skip)
{
    machine_t *machine;

    machine = allocate(1, sizeof(machine_t));
    machine->natives = create_machine_natives();
    machine->elements = create_machine_elements(machine->natives, argc, argv, skip);
    machine->effect = VALUE_EFFECT_NONE;

    return machine;
}

value_t *execute(const string_t *code, machine_t *machine)
{
    script_t *script;
    frame_t root;
    list_t *expressions;
    value_t *last;
    size_t index;

    script = parse_script(code);

    if (script->issue)
    {
        map_t *details;

        details = empty_map(hash_string, dereference_value_unsafe, 1);
        set_map_item(details, cstring_to_string("issue"), steal_string(script->issue));
        set_map_item(details, cstring_to_string("hint"), steal_string(script->hint));
        machine->effect = VALUE_EFFECT_THROW;

        script->issue = NULL;
        script->hint = NULL;
        destroy_script(script);

        return steal_map(details);
    }

    root.variables = empty_map(hash_string, dereference_value_unsafe, 8);
    expressions = script->expressions;
    last = NULL;

    for (index = 0; index < expressions->length; index++)
    {
        if (last)
        {
            dereference_value(last);
        }

        last = apply_expression(expressions->items[index], &root, machine);

        if (has_halting_effect(machine))
        {
            map_t *details;
            char *message;

            message = NULL;

            switch (machine->effect)
            {
                case VALUE_EFFECT_RETURN:
                    message = "lost return";
                    break;

                case VALUE_EFFECT_BREAK:
                    message = "lost break";
                    break;

                case VALUE_EFFECT_CONTINUE:
                    message = "lost continue";
                    break;

                case VALUE_EFFECT_THROW:
                    break;

                default:
                    crash_with_message("unsupported branch invoked");
                    break;
            }

            if (message)
            {
                dereference_value(last);
                last = steal_string(cstring_to_string(message));
            }

            details = empty_map(hash_string, dereference_value_unsafe, 1);
            set_map_item(details, cstring_to_string("issue"), steal_string(cstring_to_string("failed to execute code")));
            set_map_item(details, cstring_to_string("cause"), last);
            last->owners += 1;
            machine->effect = VALUE_EFFECT_THROW;

            dereference_value(last);
            destroy_map(root.variables);
            destroy_script(script);

            return steal_map(details);
        }
    }

    destroy_map(root.variables);
    destroy_script(script);

    if (!last)
    {
        last = new_null();
    }

    return last;
}

void destroy_machine(machine_t *machine)
{
    if (machine->natives)
    {
        destroy_map(machine->natives);
    }

    if (machine->elements)
    {
        destroy_map(machine->elements);
    }

    free(machine);
}

static value_t *apply_expression(expression_t *expression, frame_t *frame, machine_t *machine)
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
            value_t *value;

            data = expression->data;
            value = get_map_item(frame->variables, data->identifier);

            if (value)
            {
                value->owners += 1;
                return value;
            }
            else
            {
                return throw_error("absent variable", machine);
            }
        }

        case EXPRESSION_TYPE_ASSIGNMENT:
        {
            assignment_expression_data_t *data;
            value_t *value;

            data = expression->data;
            value = apply_expression(data->value, frame, machine);

            if (has_halting_effect(machine))
            {
                return value;
            }

            set_map_item(frame->variables, copy_string(data->identifier), value);
            value->owners += 1;

            return value;
        }

        case EXPRESSION_TYPE_INVOKE:
        {
            invoke_expression_data_t *data;
            frame_t descendant;
            value_t *value, *result;

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
            descendant.parent = frame;

            value = get_map_item(frame->variables, data->identifier);

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

                        last = apply_expression(expressions->items[index], &descendant, machine);

                        if (machine->effect == VALUE_EFFECT_RETURN)
                        {
                            machine->effect = VALUE_EFFECT_NONE;
                            break;
                        }
                        else if (machine->effect == VALUE_EFFECT_BREAK || machine->effect == VALUE_EFFECT_CONTINUE || machine->effect == VALUE_EFFECT_THROW)
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
                    result = throw_error("invocation error", machine);
                }
            }
            else
            {
                result = throw_error("absent variable", machine);
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
                test = apply_expression(branch->condition, frame, machine);

                if (has_halting_effect(machine))
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
                        last = apply_expression(branch->action, frame, machine);

                        break;
                    }
                }
                else
                {
                    dereference_value(test);

                    return throw_error("alien argument", machine);
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

                test = apply_expression(data->condition, frame, machine);

                if (has_halting_effect(machine))
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

                    last = apply_expression(data->action, frame, machine);

                    if (machine->effect == VALUE_EFFECT_BREAK)
                    {
                        machine->effect = VALUE_EFFECT_NONE;
                        break;
                    }
                    else if (machine->effect == VALUE_EFFECT_CONTINUE)
                    {
                        machine->effect = VALUE_EFFECT_NONE;
                        continue;
                    }
                    else if (machine->effect == VALUE_EFFECT_RETURN || machine->effect == VALUE_EFFECT_THROW)
                    {
                        break;
                    }
                }
                else
                {
                    dereference_value(test);

                    return throw_error("alien argument", machine);
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
            last = apply_expression(data->action, frame, machine);

            if (machine->effect == VALUE_EFFECT_RETURN || machine->effect == VALUE_EFFECT_BREAK || machine->effect == VALUE_EFFECT_CONTINUE)
            {
                return last;
            }
            else if (machine->effect == VALUE_EFFECT_THROW)
            {
                machine->effect = VALUE_EFFECT_NONE;

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

            if (next_argument(VALUE_TYPES_ANY, &argument, frame, machine))
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
            test = apply_expression(data->pick, frame, machine);

            if (has_halting_effect(machine))
            {
                return test;
            }

            machine->effect = VALUE_EFFECT_RETURN;

            return test;
        }

        case EXPRESSION_TYPE_BREAK:
        {
            break_expression_data_t *data;
            value_t *test;

            data = expression->data;
            test = apply_expression(data->pick, frame, machine);

            if (has_halting_effect(machine))
            {
                return test;
            }

            machine->effect = VALUE_EFFECT_BREAK;

            return test;
        }

        case EXPRESSION_TYPE_CONTINUE:
        {
            continue_expression_data_t *data;
            value_t *test;

            data = expression->data;
            test = apply_expression(data->pick, frame, machine);

            if (has_halting_effect(machine))
            {
                return test;
            }

            machine->effect = VALUE_EFFECT_CONTINUE;

            return test;
        }

        case EXPRESSION_TYPE_THROW:
        {
            throw_expression_data_t *data;
            value_t *test;

            data = expression->data;
            test = apply_expression(data->error, frame, machine);

            if (has_halting_effect(machine))
            {
                return test;
            }

            machine->effect = VALUE_EFFECT_THROW;

            return test;
        }

        case EXPRESSION_TYPE_IMPORT:
        {
            import_expression_data_t *data;
            value_t *test;

            data = expression->data;
            test = apply_expression(data->pick, frame, machine);

            if (has_halting_effect(machine))
            {
                return test;
            }

            if (test->type == VALUE_TYPE_STRING)
            {
                string_t *name;
                value_t *element;

                name = view_string(test);
                dereference_value(test);
                element = get_map_item(machine->elements, name);

                if (!element)
                {
                    return throw_error("absent import", machine);
                }

                set_map_item(frame->variables, copy_string(name), element);
                element->owners += 2;

                return element;
            }
            else if (test->type == VALUE_TYPE_LIST)
            {
                list_t *names, *elements;
                size_t index;

                names = view_list(test);
                dereference_value(test);
                elements = empty_list(dereference_value_unsafe, 1);

                for (index = 0; index < names->length; index++)
                {
                    value_t *item, *element;
                    string_t *name;

                    item = names->items[index];

                    if (item->type != VALUE_TYPE_STRING)
                    {
                        destroy_list(elements);

                        return throw_error("alien argument", machine);
                    }

                    name = view_string(item);
                    element = get_map_item(machine->elements, name);

                    if (!element)
                    {
                        destroy_list(elements);

                        return throw_error("absent import", machine);
                    }

                    set_map_item(frame->variables, copy_string(name), element);
                    element->owners += 2;

                    add_list_item(elements, element);
                }

                return steal_list(elements);
            }
            else if (test->type == VALUE_TYPE_MAP)
            {
                map_t *mappings, *elements;
                size_t index;

                mappings = view_map(test);
                dereference_value(test);
                elements = empty_map(hash_string, dereference_value_unsafe, 1);

                for (index = 0; index < mappings->capacity; index++)
                {
                    if (mappings->chains[index])
                    {
                        map_chain_t *chain;

                        for (chain = mappings->chains[index]; chain != NULL; chain = chain->next)
                        {
                            value_t *item, *element;
                            string_t *name, *alias;

                            name = chain->key;
                            element = get_map_item(machine->elements, name);

                            if (!element)
                            {
                                destroy_map(elements);

                                return throw_error("absent import", machine);
                            }

                            item = chain->value;

                            if (item->type != VALUE_TYPE_STRING)
                            {
                                destroy_map(elements);

                                return throw_error("alien argument", machine);
                            }

                            alias = view_string(item);
                            set_map_item(frame->variables, copy_string(alias), element);
                            element->owners += 2;

                            set_map_item(elements, copy_string(alias), element);
                        }
                    }
                }

                return steal_map(elements);
            }
            else
            {
                dereference_value(test);

                return throw_error("alien argument", machine);
            }
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

                last = apply_expression(expressions->items[index], frame, machine);

                if (has_halting_effect(machine))
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

        case EXPRESSION_TYPE_NATIVE:
        {
            native_expression_data_t *data;
            string_t *name;
            native_t *native;

            data = expression->data;
            name = data->name;
            native = get_map_item(machine->natives, name);

            if (native)
            {
                return native->run(frame, machine);
            }
            else
            {
                crash_with_message("unsupported branch invoked");
            }
        }

        default:
            break;
    }

    return new_null();
}

static value_t *run_add(frame_t *frame, machine_t *machine)
{
    value_t *left, *right;
    number_t sum;

    if (!next_argument(VALUE_TYPE_NUMBER, &left, frame, machine))
    {
        return left;
    }

    if (!next_argument(VALUE_TYPE_NUMBER, &right, frame, machine))
    {
        return right;
    }

    if (add_numbers(view_number(left), view_number(right), &sum) != 0)
    {
        return throw_error("arithmetic error", machine);
    }

    return steal_number(create_number(sum.value));
}

static value_t *run_subtract(frame_t *frame, machine_t *machine)
{
    value_t *left, *right;
    number_t difference;

    if (!next_argument(VALUE_TYPE_NUMBER, &left, frame, machine))
    {
        return left;
    }

    if (!next_argument(VALUE_TYPE_NUMBER, &right, frame, machine))
    {
        return right;
    }

    if (subtract_numbers(view_number(left), view_number(right), &difference) != 0)
    {
        return throw_error("arithmetic error", machine);
    }

    return steal_number(create_number(difference.value));
}

static value_t *run_multiply(frame_t *frame, machine_t *machine)
{
    value_t *left, *right;
    number_t product;

    if (!next_argument(VALUE_TYPE_NUMBER, &left, frame, machine))
    {
        return left;
    }

    if (!next_argument(VALUE_TYPE_NUMBER, &right, frame, machine))
    {
        return right;
    }

    if (multiply_numbers(view_number(left), view_number(right), &product) != 0)
    {
        return throw_error("arithmetic error", machine);
    }

    return steal_number(create_number(product.value));
}

static value_t *run_divide(frame_t *frame, machine_t *machine)
{
    value_t *left, *right;
    number_t quotient;

    if (!next_argument(VALUE_TYPE_NUMBER, &left, frame, machine))
    {
        return left;
    }

    if (!next_argument(VALUE_TYPE_NUMBER, &right, frame, machine))
    {
        return right;
    }

    if (divide_numbers(view_number(left), view_number(right), &quotient) != 0)
    {
        return throw_error("arithmetic error", machine);
    }

    return steal_number(create_number(quotient.value));
}

static value_t *run_modulo(frame_t *frame, machine_t *machine)
{
    value_t *left, *right;
    number_t remainder;

    if (!next_argument(VALUE_TYPE_NUMBER, &left, frame, machine))
    {
        return left;
    }

    if (!next_argument(VALUE_TYPE_NUMBER, &right, frame, machine))
    {
        return right;
    }

    if (modulo_numbers(view_number(left), view_number(right), &remainder) != 0)
    {
        return throw_error("arithmetic error", machine);
    }

    return steal_number(create_number(remainder.value));
}

static value_t *run_truncate(frame_t *frame, machine_t *machine)
{
    value_t *value;
    number_t *number;

    if (!next_argument(VALUE_TYPE_NUMBER, &value, frame, machine))
    {
        return value;
    }

    number = create_number(0);
    truncate_number(view_number(value), number);

    return steal_number(number);
}

static value_t *run_and(frame_t *frame, machine_t *machine)
{
    value_t *left, *right;

    if (!next_argument(VALUE_TYPE_BOOLEAN, &left, frame, machine))
    {
        return left;
    }

    if (!next_argument(VALUE_TYPE_BOOLEAN, &right, frame, machine))
    {
        return right;
    }

    return new_boolean(view_boolean(left) && view_boolean(right) ? BOOLEAN_TRUE : BOOLEAN_FALSE);
}

static value_t *run_or(frame_t *frame, machine_t *machine)
{
    value_t *left, *right;

    if (!next_argument(VALUE_TYPE_BOOLEAN, &left, frame, machine))
    {
        return left;
    }

    if (!next_argument(VALUE_TYPE_BOOLEAN, &right, frame, machine))
    {
        return right;
    }

    return new_boolean(view_boolean(left) || view_boolean(right) ? BOOLEAN_TRUE : BOOLEAN_FALSE);
}

static value_t *run_not(frame_t *frame, machine_t *machine)
{
    value_t *value;

    if (!next_argument(VALUE_TYPE_BOOLEAN, &value, frame, machine))
    {
        return value;
    }

    return new_boolean(!view_boolean(value) ? BOOLEAN_TRUE : BOOLEAN_FALSE);
}

static value_t *run_precedes(frame_t *frame, machine_t *machine)
{
    value_t *left, *right;

    if (!next_argument(VALUE_TYPES_ANY, &left, frame, machine))
    {
        return left;
    }

    if (!next_argument(VALUE_TYPES_ANY, &right, frame, machine))
    {
        return right;
    }

    return new_boolean(compare_values(left, right) < 0 ? BOOLEAN_TRUE : BOOLEAN_FALSE);
}

static value_t *run_succeeds(frame_t *frame, machine_t *machine)
{
    value_t *left, *right;

    if (!next_argument(VALUE_TYPES_ANY, &left, frame, machine))
    {
        return left;
    }

    if (!next_argument(VALUE_TYPES_ANY, &right, frame, machine))
    {
        return right;
    }

    return new_boolean(compare_values(left, right) > 0 ? BOOLEAN_TRUE : BOOLEAN_FALSE);
}

static value_t *run_equals(frame_t *frame, machine_t *machine)
{
    value_t *left, *right;

    if (!next_argument(VALUE_TYPES_ANY, &left, frame, machine))
    {
        return left;
    }

    if (!next_argument(VALUE_TYPES_ANY, &right, frame, machine))
    {
        return right;
    }

    return new_boolean(compare_values(left, right) == 0 ? BOOLEAN_TRUE : BOOLEAN_FALSE);
}

static value_t *run_write(frame_t *frame, machine_t *machine)
{
    value_t *message, *file;
    FILE *handle;
    int closable, flushable;
    string_t *string;

    if (!next_argument(VALUE_TYPE_STRING, &message, frame, machine))
    {
        return message;
    }

    if (!next_argument(VALUE_TYPE_NUMBER | VALUE_TYPE_STRING, &file, frame, machine))
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
            number_t *streamID, inID, outID, errID;

            streamID = view_number(file);
            integer_to_number(0, &inID);
            integer_to_number(1, &outID);
            integer_to_number(2, &errID);

            if (compare_numbers(streamID, &inID) == 0)
            {
                handle = stdin;
            }
            else if (compare_numbers(streamID, &outID) == 0)
            {
                handle = stdout;
            }
            else if (compare_numbers(streamID, &errID) == 0)
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
        return throw_error("absent file", machine);
    }

    if (string->length > 0)
    {
        fwrite(string->bytes, sizeof(char), string->length, handle);
    }

    if (ferror(handle))
    {
        if (closable)
        {
            fclose(handle);
        }

        return throw_error("io error", machine);
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

static value_t *run_read(frame_t *frame, machine_t *machine)
{
    value_t *file, *until;
    FILE *handle;
    int closable, terminated;
    string_t *terminator;
    char *bytes;
    size_t fill, length;

    if (!next_argument(VALUE_TYPE_NUMBER | VALUE_TYPE_STRING, &file, frame, machine))
    {
        return file;
    }

    if (!next_argument(VALUE_TYPE_NULL | VALUE_TYPE_STRING, &until, frame, machine))
    {
        return until;
    }

    terminated = until->type == VALUE_TYPE_STRING;
    terminator = terminated ? view_string(until) : NULL;

    if (terminated && terminator->length != 1)
    {
        return throw_error("damaged argument", machine);
    }

    closable = 0;

    switch (file->type)
    {
        case VALUE_TYPE_NUMBER:
        {
            number_t *streamID, inID, outID, errID;

            streamID = view_number(file);
            integer_to_number(0, &inID);
            integer_to_number(1, &outID);
            integer_to_number(2, &errID);

            if (compare_numbers(streamID, &inID) == 0)
            {
                handle = stdin;
            }
            else if (compare_numbers(streamID, &outID) == 0)
            {
                handle = stdout;
            }
            else if (compare_numbers(streamID, &errID) == 0)
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
        return throw_error("absent file", machine);
    }

    fill = 0;
    length = 256;
    bytes = allocate(length, sizeof(char));

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

            return throw_error("io error", machine);
        }

        if (symbol == EOF || (terminated && terminator->bytes[0] == symbol))
        {
            break;
        }

        bytes[fill++] = symbol;

        if (fill == length)
        {
            char *swap;

            if (!can_fit_both(length, length))
            {
                crash_with_message("oversized memory requested");
            }

            length *= 2;
            swap = allocate(length, sizeof(char));
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

static value_t *run_delete(frame_t *frame, machine_t *machine)
{
    value_t *file;

    if (!next_argument(VALUE_TYPE_NUMBER | VALUE_TYPE_STRING, &file, frame, machine))
    {
        return file;
    }

    switch (file->type)
    {
        case VALUE_TYPE_NUMBER:
        {
            return throw_error("io error", machine);
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
                return throw_error("io error", machine);
            }
        }

        default:
            crash_with_message("unsupported branch invoked");
            return new_null();
    }
}

static value_t *run_query(frame_t *frame, machine_t *machine)
{
    value_t *key;
    char *value, *cstring;

    if (!next_argument(VALUE_TYPE_STRING, &key, frame, machine))
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
        return throw_error("absent environment variable", machine);
    }
}

static value_t *run_evaluate(frame_t *frame, machine_t *machine)
{
    value_t *code;

    if (!next_argument(VALUE_TYPE_STRING, &code, frame, machine))
    {
        return code;
    }

    return execute(view_string(code), machine);
}

static value_t *run_freeze(frame_t *frame, machine_t *machine)
{
    value_t *value;

    if (!next_argument(VALUE_TYPES_ANY, &value, frame, machine))
    {
        return value;
    }

    return steal_string(represent_value(value));
}

static value_t *run_thaw(frame_t *frame, machine_t *machine)
{
    value_t *code, *value;
    script_t *script;
    expression_t *expression;
    literal_expression_data_t *data;

    if (!next_argument(VALUE_TYPE_STRING, &code, frame, machine))
    {
        return code;
    }

    script = parse_script(view_string(code));

    if (script->issue || script->expressions->length != 1)
    {
        destroy_script(script);

        return throw_error("cast error", machine);
    }

    expression = script->expressions->items[0];

    if (expression->type != EXPRESSION_TYPE_LITERAL)
    {
        destroy_script(script);

        return throw_error("cast error", machine);
    }

    data = expression->data;
    value = data->value;
    data->value = NULL;
    destroy_script(script);

    return value;
}

static value_t *run_type(frame_t *frame, machine_t *machine)
{
    value_t *value;

    if (!next_argument(VALUE_TYPES_ANY, &value, frame, machine))
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

static value_t *run_cast(frame_t *frame, machine_t *machine)
{
    value_t *value, *type;
    string_t *pattern;

    if (!next_argument(VALUE_TYPES_ANY, &value, frame, machine))
    {
        return value;
    }

    if (!next_argument(VALUE_TYPE_STRING, &type, frame, machine))
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
                return throw_error("cast error", machine);
            }
        }
        else
        {
            return throw_error("cast error", machine);
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
                return throw_error("cast error", machine);
            }
        }
        else
        {
            return throw_error("cast error", machine);
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
                return throw_error("cast error", machine);
            }

            return steal_number(create_number(number.value));
        }
        else
        {
            return throw_error("cast error", machine);
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
            return throw_error("cast error", machine);
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
            return throw_error("cast error", machine);
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
            return throw_error("cast error", machine);
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
            return throw_error("cast error", machine);
        }
    }
    else
    {
        return throw_error("damaged argument", machine);
    }
}

static value_t *run_get(frame_t *frame, machine_t *machine)
{
    value_t *collection, *key;

    if (!next_argument(VALUE_TYPES_COLLECTION, &collection, frame, machine))
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

            if (!next_argument(VALUE_TYPE_NUMBER, &key, frame, machine))
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
                return throw_error("absent key", machine);
            }

            bytes = allocate(1, sizeof(char));
            bytes[0] = string->bytes[number - 1];

            return steal_string(create_string(bytes, 1));
        }

        case VALUE_TYPE_LIST:
        {
            list_t *list;
            size_t index, cursor;
            int number;

            if (!next_argument(VALUE_TYPE_NUMBER, &key, frame, machine))
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
                return throw_error("absent key", machine);
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

            if (!next_argument(VALUE_TYPE_STRING, &key, frame, machine))
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
                return throw_error("absent key", machine);
            }
        }

        default:
            crash_with_message("unsupported branch invoked");
            return new_null();
    }
}

static value_t *run_set(frame_t *frame, machine_t *machine)
{
    value_t *collection, *key, *item;

    if (!next_argument(VALUE_TYPES_COLLECTION, &collection, frame, machine))
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

            if (!next_argument(VALUE_TYPE_NUMBER, &key, frame, machine))
            {
                return key;
            }

            if (!next_argument(VALUE_TYPE_STRING, &item, frame, machine))
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
                return throw_error("absent key", machine);
            }

            beforeLength = index - 1;
            afterLength = source->length - beforeLength - 1;

            if (!can_fit_both(beforeLength, middle->length))
            {
                crash_with_message("oversized memory requested");
            }

            destinationLength = beforeLength + middle->length;

            if (!can_fit_both(destinationLength, afterLength))
            {
                crash_with_message("oversized memory requested");
            }

            destinationLength += afterLength;

            destination = allocate(destinationLength, sizeof(char));

            if (beforeLength > 0)
            {
                memcpy(destination, source->bytes, beforeLength);
            }

            if (middle->length > 0)
            {
                memcpy(destination + beforeLength, middle->bytes, middle->length);
            }

            if (afterLength > 0)
            {
                memcpy(destination + beforeLength + middle->length, source->bytes + beforeLength + 1, afterLength);
            }

            return steal_string(create_string(destination, destinationLength));
        }

        case VALUE_TYPE_LIST:
        {
            list_t *source, *destination;
            size_t index, cursor;
            int number;

            if (!next_argument(VALUE_TYPE_NUMBER, &key, frame, machine))
            {
                return key;
            }

            if (!next_argument(VALUE_TYPES_ANY, &item, frame, machine))
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
                return throw_error("absent key", machine);
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

            if (!next_argument(VALUE_TYPE_STRING, &key, frame, machine))
            {
                return key;
            }

            if (!next_argument(VALUE_TYPES_ANY, &item, frame, machine))
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

static value_t *run_unset(frame_t *frame, machine_t *machine)
{
    value_t *collection, *key;

    if (!next_argument(VALUE_TYPES_COLLECTION, &collection, frame, machine))
    {
        return collection;
    }

    switch (collection->type)
    {
        case VALUE_TYPE_STRING:
        {
            string_t *source;
            int number;

            if (!next_argument(VALUE_TYPE_NUMBER, &key, frame, machine))
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
                return throw_error("absent key", machine);
            }

            if (source->length > 1)
            {
                char *bytes;

                bytes = allocate(source->length, sizeof(char));
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

            if (!next_argument(VALUE_TYPE_NUMBER, &key, frame, machine))
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
                return throw_error("absent key", machine);
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

            if (!next_argument(VALUE_TYPE_STRING, &key, frame, machine))
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

static value_t *run_merge(frame_t *frame, machine_t *machine)
{
    value_t *left, *right;

    if (!next_argument(VALUE_TYPES_COLLECTION, &left, frame, machine))
    {
        return left;
    }

    switch (left->type)
    {
        case VALUE_TYPE_STRING:
        {
            if (!next_argument(VALUE_TYPE_STRING, &right, frame, machine))
            {
                return right;
            }

            return steal_string(merge_strings(view_string(left), view_string(right)));
        }

        case VALUE_TYPE_LIST:
        {
            list_t *source, *destination;
            size_t index;

            if (!next_argument(VALUE_TYPE_LIST, &right, frame, machine))
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

            if (!next_argument(VALUE_TYPE_MAP, &right, frame, machine))
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

static value_t *run_length(frame_t *frame, machine_t *machine)
{
    value_t *collection;
    size_t length;
    number_t number;

    if (!next_argument(VALUE_TYPES_COLLECTION, &collection, frame, machine))
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
        return throw_error("constraint error", machine);
    }

    return steal_number(create_number(number.value));
}

static value_t *run_keys(frame_t *frame, machine_t *machine)
{
    value_t *collection;

    if (!next_argument(VALUE_TYPES_COLLECTION, &collection, frame, machine))
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
                return throw_error("constraint error", machine);
            }

            keys = empty_list(dereference_value_unsafe, 1);

            for (index = 0; index < string->length; index++)
            {
                number_t key;

                if (index + 1 >= PORTABLE_INT_LIMIT || integer_to_number(index + 1, &key) != 0)
                {
                    destroy_list(keys);

                    return throw_error("constraint error", machine);
                }

                add_list_item(keys, steal_number(create_number(key.value)));
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
                return throw_error("constraint error", machine);
            }

            keys = empty_list(dereference_value_unsafe, 1);

            for (index = 0; index < length; index++)
            {
                number_t key;

                if (index + 1 >= PORTABLE_INT_LIMIT || integer_to_number(index + 1, &key) != 0)
                {
                    destroy_list(keys);

                    return throw_error("constraint error", machine);
                }

                add_list_item(keys, steal_number(create_number(key.value)));
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

static value_t *run_sort(frame_t *frame, machine_t *machine)
{
    value_t *collection, *direction;
    list_t *source, *destination;
    int ascending;
    size_t index;

    if (!next_argument(VALUE_TYPE_LIST, &collection, frame, machine))
    {
        return collection;
    }

    if (!next_argument(VALUE_TYPE_STRING, &direction, frame, machine))
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
        return throw_error("damaged argument", machine);
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

static map_t *create_machine_natives()
{
    map_t *map;

    map = empty_map(hash_string, destroy_native_unsafe, 32);

    set_map_item(map, cstring_to_string("add"), create_native(run_add));
    set_map_item(map, cstring_to_string("subtract"), create_native(run_subtract));
    set_map_item(map, cstring_to_string("multiply"), create_native(run_multiply));
    set_map_item(map, cstring_to_string("divide"), create_native(run_divide));
    set_map_item(map, cstring_to_string("modulo"), create_native(run_modulo));
    set_map_item(map, cstring_to_string("truncate"), create_native(run_truncate));
    set_map_item(map, cstring_to_string("and"), create_native(run_and));
    set_map_item(map, cstring_to_string("or"), create_native(run_or));
    set_map_item(map, cstring_to_string("not"), create_native(run_not));
    set_map_item(map, cstring_to_string("precedes"), create_native(run_precedes));
    set_map_item(map, cstring_to_string("succeeds"), create_native(run_succeeds));
    set_map_item(map, cstring_to_string("equals"), create_native(run_equals));
    set_map_item(map, cstring_to_string("write"), create_native(run_write));
    set_map_item(map, cstring_to_string("read"), create_native(run_read));
    set_map_item(map, cstring_to_string("delete"), create_native(run_delete));
    set_map_item(map, cstring_to_string("query"), create_native(run_query));
    set_map_item(map, cstring_to_string("evaluate"), create_native(run_evaluate));
    set_map_item(map, cstring_to_string("freeze"), create_native(run_freeze));
    set_map_item(map, cstring_to_string("thaw"), create_native(run_thaw));
    set_map_item(map, cstring_to_string("type"), create_native(run_type));
    set_map_item(map, cstring_to_string("cast"), create_native(run_cast));
    set_map_item(map, cstring_to_string("get"), create_native(run_get));
    set_map_item(map, cstring_to_string("set"), create_native(run_set));
    set_map_item(map, cstring_to_string("unset"), create_native(run_unset));
    set_map_item(map, cstring_to_string("merge"), create_native(run_merge));
    set_map_item(map, cstring_to_string("length"), create_native(run_length));
    set_map_item(map, cstring_to_string("keys"), create_native(run_keys));
    set_map_item(map, cstring_to_string("sort"), create_native(run_sort));

    return map;
}

static map_t *create_machine_elements(const map_t *natives, int argc, char **argv, int skip)
{
    map_t *map;
    list_t *inputs;
    size_t index;

    map = empty_map(hash_string, dereference_value_unsafe, natives->capacity);

    for (index = 0; index < natives->capacity; index++)
    {
        if (natives->chains[index])
        {
            map_chain_t *chain;

            for (chain = natives->chains[index]; chain != NULL; chain = chain->next)
            {
                string_t *name;

                name = chain->key;
                set_map_item(map, copy_string(name), create_element_function(copy_string(name)));
            }
        }
    }

    inputs = empty_list(dereference_value_unsafe, 1);

    while (skip > 0)
    {
        argc--;
        argv++;
        skip--;
    }

    while (argc > 0)
    {
        add_list_item(inputs, steal_string(cstring_to_string(*argv)));
        argc--;
        argv++;
    }

    set_map_item(map, cstring_to_string("inputs"), steal_list(inputs));

    return map;
}

static value_t *create_element_function(string_t *name)
{
    list_t *expressions;
    expression_t *expression;
    native_expression_data_t *data;
    string_t *source;

    expression = allocate(1, sizeof(expression_t));
    expression->type = EXPRESSION_TYPE_NATIVE;
    data = allocate(1, sizeof(native_expression_data_t));
    data->name = name;
    expression->data = data;
    expressions = empty_list(destroy_expression_unsafe, 1);
    add_list_item(expressions, expression);

    source = cstring_to_string("<import \"");
    extend_string_by_string(source, name);
    extend_string_by_cstring(source, "\" from core>");

    return steal_function(create_function(expressions, source));
}

static native_t *create_native(value_t *(*run)(frame_t *, machine_t *))
{
    native_t *native;

    native = allocate(1, sizeof(native_t));
    native->run = run;

    return native;
}

static int next_argument(int types, value_t **out, frame_t *frame, machine_t *machine)
{
    value_t *result;

    if (!has_next_argument(frame))
    {
        (*out) = throw_error("absent argument", machine);
        return 0;
    }

    result = apply_expression(frame->arguments.expressions->items[frame->arguments.index], frame->parent, machine);
    frame->arguments.evaluated[frame->arguments.index] = result;
    frame->arguments.index += 1;

    if (has_halting_effect(machine))
    {
        result->owners += 1;
        (*out) = result;
        return 0;
    }

    if (!(types & result->type))
    {
        (*out) = throw_error("alien argument", machine);
        return 0;
    }

    (*out) = result;
    return 1;
}

static int has_next_argument(const frame_t *frame)
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

static value_t *throw_error(const char *message, machine_t *machine)
{
    machine->effect = VALUE_EFFECT_THROW;
    return steal_string(cstring_to_string(message));
}

static void destroy_expression_unsafe(void *expression)
{
    destroy_expression((expression_t *) expression);
}

static void destroy_native_unsafe(void *native)
{
    free(native);
}

static void dereference_value_unsafe(void *value)
{
    dereference_value(value);
}

static int has_halting_effect(const machine_t *machine)
{
    value_effect_t effect;

    effect = machine->effect;

    return effect == VALUE_EFFECT_RETURN
        || effect == VALUE_EFFECT_BREAK
        || effect == VALUE_EFFECT_CONTINUE
        || effect == VALUE_EFFECT_THROW;
}
