#include <stdlib.h>
#include <string.h>
#include "parse.h"
#include "lex.h"
#include "utility.h"

typedef struct
{
    scanner_t scanner;
    token_t present;
    token_t future;
    int hasPresent;
    int hasFuture;
    int ahead;
} capsule_t;

static expression_t *read_any_expression(capsule_t *capsule);
static expression_t *read_list_expression(capsule_t *capsule);
static expression_t *read_map_expression(capsule_t *capsule);
static expression_t *read_function_expression(capsule_t *capsule_t);
static expression_t *read_assignment_expression(capsule_t *capsule, identifier_t *identifier);
static expression_t *read_invoke_expression(capsule_t *capsule, identifier_t *identifier);
static expression_t *read_branch_expression(capsule_t *capsule);
static expression_t *read_loop_expression(capsule_t *capsule);
static expression_t *read_catch_expression(capsule_t *capsule);
static expression_t *read_return_expression(capsule_t *capsule);
static expression_t *read_break_expression(capsule_t *capsule);
static expression_t *read_continue_expression(capsule_t *capsule);
static expression_t *read_throw_expression(capsule_t *capsule);
static expression_t *read_snippet_expression(capsule_t *capsule);
static conditional_branch_t *read_conditional_branch(capsule_t *capsule);
static token_t *peek_token(capsule_t *capsule);
static token_t *next_token(capsule_t *capsule);
static token_t *scan_token(scanner_t *scanner);
static string_t *substring_using_token(const string_t *code, const token_t *token);
static string_t *substring_to_newline(const string_t *code, size_t start, size_t limit);
static identifier_t *parse_identifier(const string_t *code, const token_t *token);
static string_t *unescape_string(const string_t *code, const token_t *token);
static char is_symbol_token(char symbol, const string_t *code, const token_t *token);
static expression_t *create_unknown_expression();
static expression_t *create_literal_expression(value_t *value);
static expression_t *create_reference_expression(identifier_t *identifier);
static expression_t *create_assignment_expression(identifier_t *identifier, expression_t *value);
static expression_t *create_invoke_expression(identifier_t *identifier, list_t *arguments);
static expression_t *create_branch_expression(list_t *branches);
static expression_t *create_loop_expression(expression_t *condition, expression_t *action);
static expression_t *create_catch_expression(expression_t *action);
static expression_t *create_argument_expression();
static expression_t *create_return_expression(expression_t *pick);
static expression_t *create_break_expression(expression_t *pick);
static expression_t *create_continue_expression(expression_t *pick);
static expression_t *create_throw_expression(expression_t *error);
static expression_t *create_snippet_expression(list_t *expressions);
static expression_t *create_expression(expression_type_t type, void *data);
static void destroy_expression_unsafe(void *expression);
static void dereference_value_unsafe(void *value);
static void destroy_conditional_branch(conditional_branch_t *branch);
static void destroy_conditional_branch_unsafe(void *branch);

script_t *parse_script(string_t *code)
{
    capsule_t capsule;
    script_t *script;
    list_t *expressions;
    string_t *errorMessage;
    string_t *hintMessage;

    capsule.hasPresent = 0;
    capsule.hasFuture = 0;
    capsule.ahead = 0;
    start_scanner(&capsule.scanner, code);

    expressions = empty_list(destroy_expression_unsafe, 1);
    errorMessage = NULL;
    hintMessage = NULL;

    while (capsule.scanner.state == SCANNER_STATE_RUNNING)
    {
        expression_t *expression;

        expression = read_any_expression(&capsule);

        if (expression)
        {
            if (expression->type == EXPRESSION_TYPE_UNKNOWN && capsule.scanner.state != SCANNER_STATE_ERRORED)
            {
                destroy_list(expressions);
                destroy_expression(expression);
                expressions = NULL;
                errorMessage = cstring_to_string("failed to parse code");
                hintMessage = substring_to_newline(capsule.scanner.code, capsule.present.start, 50);
                break;
            }

            add_list_item(expressions, expression);
        }
    }

    if (capsule.scanner.state == SCANNER_STATE_ERRORED)
    {
        errorMessage = cstring_to_string("failed to lex code");
        hintMessage = substring_to_newline(capsule.scanner.code, capsule.scanner.token.start, 50);
    }

    script = allocate(1, sizeof(script_t));
    script->expressions = expressions;
    script->errorMessage = errorMessage;
    script->hintMessage = hintMessage;

    return script;
}

void destroy_script(script_t *script)
{
    if (script->errorMessage)
    {
        destroy_string(script->errorMessage);
    }

    if (script->hintMessage)
    {
        destroy_string(script->hintMessage);
    }

    if (script->expressions)
    {
        destroy_list(script->expressions);
    }

    free(script);
}

void destroy_expression(expression_t *expression)
{
    if (expression->data)
    {
        switch (expression->type)
        {
            case EXPRESSION_TYPE_LITERAL:
            {
                literal_expression_data_t *data;

                data = expression->data;

                if (data->value)
                {
                    dereference_value(data->value);
                }

                free(data);
                break;
            }

            case EXPRESSION_TYPE_REFERENCE:
            {
                reference_expression_data_t *data;

                data = expression->data;

                if (data->identifier)
                {
                    destroy_identifier(data->identifier);
                }

                free(data);
                break;
            }

            case EXPRESSION_TYPE_ASSIGNMENT:
            {
                assignment_expression_data_t *data;

                data = expression->data;

                if (data->identifier)
                {
                    destroy_identifier(data->identifier);
                }

                if (data->value)
                {
                    destroy_expression(data->value);
                }

                free(data);
                break;
            }

            case EXPRESSION_TYPE_INVOKE:
            {
                invoke_expression_data_t *data;

                data = expression->data;

                if (data->identifier)
                {
                    destroy_identifier(data->identifier);
                }

                if (data->arguments)
                {
                    destroy_list(data->arguments);
                }

                free(data);
                break;
            }

            case EXPRESSION_TYPE_BRANCH:
            {
                branch_expression_data_t *data;

                data = expression->data;

                if (data->branches)
                {
                    destroy_list(data->branches);
                }

                free(data);
                break;
            }

            case EXPRESSION_TYPE_LOOP:
            {
                loop_expression_data_t *data;

                data = expression->data;

                if (data->condition)
                {
                    destroy_expression(data->condition);
                }

                if (data->action)
                {
                    destroy_expression(data->action);
                }

                free(data);
                break;
            }

            case EXPRESSION_TYPE_CATCH:
            {
                catch_expression_data_t *data;

                data = expression->data;

                if (data->action)
                {
                    destroy_expression(data->action);
                }

                free(data);
                break;
            }

            case EXPRESSION_TYPE_RETURN:
            {
                return_expression_data_t *data;

                data = expression->data;

                if (data->pick)
                {
                    destroy_expression(data->pick);
                }

                free(data);
                break;
            }

            case EXPRESSION_TYPE_BREAK:
            {
                break_expression_data_t *data;

                data = expression->data;

                if (data->pick)
                {
                    destroy_expression(data->pick);
                }

                free(data);
                break;
            }

            case EXPRESSION_TYPE_CONTINUE:
            {
                continue_expression_data_t *data;

                data = expression->data;

                if (data->pick)
                {
                    destroy_expression(data->pick);
                }

                free(data);
                break;
            }

            case EXPRESSION_TYPE_THROW:
            {
                throw_expression_data_t *data;

                data = expression->data;

                if (data->error)
                {
                    destroy_expression(data->error);
                }

                free(data);
                break;
            }

            case EXPRESSION_TYPE_SNIPPET:
            {
                snippet_expression_data_t *data;

                data = expression->data;

                if (data->expressions)
                {
                    destroy_list(data->expressions);
                }

                free(data);
                break;
            }

            default:
                crash_with_message("unsupported branch invoked");
                break;
        }
    }

    free(expression);
}

void destroy_identifier(identifier_t *identifier)
{
    if (identifier->name)
    {
        destroy_string(identifier->name);
    }

    free(identifier);
}

static expression_t *read_any_expression(capsule_t *capsule)
{
    token_t *token;

    token = next_token(capsule);

    if (!token)
    {
        return NULL;
    }

    if (token->type == TOKEN_TYPE_NUMBER)
    {
        number_t value;
        string_t *text;

        text = substring_using_token(capsule->scanner.code, token);

        if (string_to_number(text, &value) != 0)
        {
            destroy_string(text);

            return create_unknown_expression();
        }

        destroy_string(text);

        return create_literal_expression(new_number(value));
    }
    else if (token->type == TOKEN_TYPE_STRING)
    {
        string_t *string;

        string = unescape_string(capsule->scanner.code, token);

        return string ? create_literal_expression(steal_string(string)) : create_unknown_expression();
    }
    else if (token->type == TOKEN_TYPE_IDENTIFIER)
    {
        identifier_t *identifier;
        token_t *optional;

        identifier = parse_identifier(capsule->scanner.code, token);

        if (!identifier)
        {
            return create_unknown_expression();
        }

        optional = peek_token(capsule);

        if (is_symbol_token('=', capsule->scanner.code, optional))
        {
            next_token(capsule);

            return read_assignment_expression(capsule, identifier);
        }
        else if (is_symbol_token('(', capsule->scanner.code, optional))
        {
            next_token(capsule);

            return read_invoke_expression(capsule, identifier);
        }
        else
        {
            return create_reference_expression(identifier);
        }
    }
    else if (token->type == TOKEN_TYPE_KEYWORD)
    {
        expression_t *expression;
        string_t *keyword;

        keyword = substring_using_token(capsule->scanner.code, token);

        if (is_keyword_match(keyword, "null"))
        {
            expression = create_literal_expression(new_null());
        }
        else if (is_keyword_match(keyword, "false"))
        {
            expression = create_literal_expression(new_boolean(BOOLEAN_FALSE));
        }
        else if (is_keyword_match(keyword, "true"))
        {
            expression = create_literal_expression(new_boolean(BOOLEAN_TRUE));
        }
        else if (is_keyword_match(keyword, "if"))
        {
            expression = read_branch_expression(capsule);
        }
        else if (is_keyword_match(keyword, "while"))
        {
            expression = read_loop_expression(capsule);
        }
        else if (is_keyword_match(keyword, "catch"))
        {
            expression = read_catch_expression(capsule);
        }
        else if (is_keyword_match(keyword, "argument"))
        {
            expression = create_argument_expression();
        }
        else if (is_keyword_match(keyword, "return"))
        {
            expression = read_return_expression(capsule);
        }
        else if (is_keyword_match(keyword, "break"))
        {
            expression = read_break_expression(capsule);
        }
        else if (is_keyword_match(keyword, "continue"))
        {
            expression = read_continue_expression(capsule);
        }
        else if (is_keyword_match(keyword, "throw"))
        {
            expression = read_throw_expression(capsule);
        }
        else
        {
            crash_with_message("unsupported branch invoked");
            return NULL;
        }

        destroy_string(keyword);

        return expression;
    }
    else if (token->type == TOKEN_TYPE_SYMBOL)
    {
        if (is_symbol_token('[', capsule->scanner.code, token))
        {
            return read_list_expression(capsule);
        }
        else if (is_symbol_token('{', capsule->scanner.code, token))
        {
            return read_map_expression(capsule);
        }
        else if (is_symbol_token('<', capsule->scanner.code, token))
        {
            return read_function_expression(capsule);
        }
        else if (is_symbol_token('\\', capsule->scanner.code, token))
        {
            return read_snippet_expression(capsule);
        }
    }

    return create_unknown_expression();
}

static expression_t *read_list_expression(capsule_t *capsule)
{
    list_t *items;
    int ready;

    items = empty_list(dereference_value_unsafe, 1);
    ready = 1;

    while (1)
    {
        token_t *optional;

        optional = peek_token(capsule);

        if (is_symbol_token(']', capsule->scanner.code, optional))
        {
            next_token(capsule);

            break;
        }
        else if (is_symbol_token(',', capsule->scanner.code, optional))
        {
            next_token(capsule);

            if (ready)
            {
                destroy_list(items);

                return create_unknown_expression();
            }

            ready = 1;
        }
        else
        {
            expression_t *item;
            literal_expression_data_t *data;

            if (!ready)
            {
                destroy_list(items);

                return create_unknown_expression();
            }

            item = read_any_expression(capsule);

            if (!item || item->type != EXPRESSION_TYPE_LITERAL)
            {
                if (item)
                {
                    destroy_expression(item);
                }

                destroy_list(items);

                return create_unknown_expression();
            }

            ready = 0;
            data = (literal_expression_data_t *) item->data;
            add_list_item(items, data->value);
            data->value = NULL;
            destroy_expression(item);
        }
    }

    return create_literal_expression(steal_list(items));
}

static expression_t *read_map_expression(capsule_t *capsule)
{
    map_t *mappings;
    int mode; /* 0:wants_key, 1:wants_glue, 2:wants_value, 3:wants_delimiter_or_end */
    string_t *hold;

    mappings = empty_map(hash_string, dereference_value_unsafe, 1);
    mode = 0;
    hold = NULL;

    while (1)
    {
        token_t *optional;

        optional = peek_token(capsule);

        if (mode == 0)
        {
            expression_t *key;
            literal_expression_data_t *data;
            value_t *steal;

            if (is_symbol_token('}', capsule->scanner.code, optional))
            {
                next_token(capsule);

                break;
            }

            key = read_any_expression(capsule);

            if (!key || key->type != EXPRESSION_TYPE_LITERAL)
            {
                if (key)
                {
                    destroy_expression(key);
                }

                destroy_map(mappings);

                return create_unknown_expression();
            }

            mode = 1;
            data = key->data;
            steal = data->value;

            if (steal->type != VALUE_TYPE_STRING)
            {
                destroy_expression(key);
                destroy_map(mappings);

                return create_unknown_expression();
            }

            hold = view_string(steal);
            steal->data = NULL;
            destroy_expression(key);
        }
        else if (mode == 1)
        {
            if (is_symbol_token(':', capsule->scanner.code, optional))
            {
                mode = 2;
                next_token(capsule);

                continue;
            }
            else
            {
                destroy_string(hold);
                destroy_map(mappings);

                return create_unknown_expression();
            }
        }
        else if (mode == 2)
        {
            expression_t *value;
            literal_expression_data_t *data;

            value = read_any_expression(capsule);

            if (!value || value->type != EXPRESSION_TYPE_LITERAL)
            {
                if (value)
                {
                    destroy_expression(value);
                }

                destroy_string(hold);
                destroy_map(mappings);

                return create_unknown_expression();
            }

            mode = 3;
            data = value->data;
            set_map_item(mappings, hold, data->value);
            hold = NULL;
            data->value = NULL;
            destroy_expression(value);
        }
        else if (mode == 3)
        {
            mode = 0;

            if (is_symbol_token(',', capsule->scanner.code, optional))
            {
                next_token(capsule);
            }
        }
    }

    return create_literal_expression(steal_map(mappings));
}

static expression_t *read_function_expression(capsule_t *capsule)
{
    function_t *function;
    list_t *expressions;
    size_t start, end;
    char *bytes;
    size_t length;

    start = capsule->present.start;

    expressions = empty_list(destroy_expression_unsafe, 1);

    while (1)
    {
        expression_t *expression;
        token_t *optional;

        optional = peek_token(capsule);

        if (is_symbol_token('>', capsule->scanner.code, optional))
        {
            next_token(capsule);

            break;
        }

        expression = read_any_expression(capsule);

        if (!expression || expression->type == EXPRESSION_TYPE_UNKNOWN)
        {
            destroy_list(expressions);

            return expression != NULL ? expression : create_unknown_expression();
        }

        add_list_item(expressions, expression);
    }

    end = capsule->present.end;
    length = end - start;
    bytes = allocate(length, sizeof(char));
    memcpy(bytes, capsule->scanner.code->bytes + start, length);
    function = create_function(expressions, create_string(bytes, length));

    return create_literal_expression(steal_function(function));
}

static expression_t *read_assignment_expression(capsule_t *capsule, identifier_t *identifier)
{
    expression_t *value;

    value = read_any_expression(capsule);

    if (!value || value->type == EXPRESSION_TYPE_UNKNOWN)
    {
        destroy_identifier(identifier);

        return value != NULL ? value : create_unknown_expression();
    }

    return create_assignment_expression(identifier, value);
}

static expression_t *read_invoke_expression(capsule_t *capsule, identifier_t *identifier)
{
    list_t *arguments;
    int ready;

    arguments = empty_list(destroy_expression_unsafe, 1);
    ready = 1;

    while (1)
    {
        token_t *optional;

        optional = peek_token(capsule);

        if (is_symbol_token(')', capsule->scanner.code, optional))
        {
            next_token(capsule);
            break;
        }
        else if (is_symbol_token(',', capsule->scanner.code, optional))
        {
            next_token(capsule);

            if (ready)
            {
                destroy_identifier(identifier);
                destroy_list(arguments);

                return create_unknown_expression();
            }

            ready = 1;
        }
        else
        {
            expression_t *argument;

            if (!ready)
            {
                destroy_identifier(identifier);
                destroy_list(arguments);

                return create_unknown_expression();
            }

            argument = read_any_expression(capsule);

            if (!argument || argument->type == EXPRESSION_TYPE_UNKNOWN)
            {
                destroy_identifier(identifier);
                destroy_list(arguments);

                return argument != NULL ? argument : create_unknown_expression();
            }

            ready = 0;
            add_list_item(arguments, argument);
        }
    }

    return create_invoke_expression(identifier, arguments);
}

static expression_t *read_branch_expression(capsule_t *capsule)
{
    list_t *branches;
    conditional_branch_t *branch;
    token_t *optional;

    branch = read_conditional_branch(capsule);

    if (!branch)
    {
        return create_unknown_expression();
    }

    branches = empty_list(destroy_conditional_branch_unsafe, 1);
    add_list_item(branches, branch);

    while (1)
    {
        optional = peek_token(capsule);

        if (optional && optional->type == TOKEN_TYPE_KEYWORD)
        {
            string_t *keyword;

            keyword = substring_using_token(capsule->scanner.code, optional);

            if (is_keyword_match(keyword, "else"))
            {
                destroy_string(keyword);
                next_token(capsule);

                optional = peek_token(capsule);

                if (optional && optional->type == TOKEN_TYPE_KEYWORD)
                {
                    keyword = substring_using_token(capsule->scanner.code, optional);

                    if (is_keyword_match(keyword, "if"))
                    {
                        destroy_string(keyword);
                        next_token(capsule);
                        branch = read_conditional_branch(capsule);

                        if (!branch)
                        {
                            destroy_list(branches);

                            return create_unknown_expression();
                        }

                        add_list_item(branches, branch);
                    }
                    else
                    {
                        destroy_string(keyword);
                        destroy_list(branches);

                        return create_unknown_expression();
                    }
                }
                else
                {
                    destroy_list(branches);

                    return create_unknown_expression();
                }
            }
            else if (is_keyword_match(keyword, "otherwise"))
            {
                expression_t *action;

                destroy_string(keyword);
                next_token(capsule);

                action = read_any_expression(capsule);

                if (!action || action->type == EXPRESSION_TYPE_UNKNOWN)
                {
                    destroy_list(branches);

                    return action != NULL ? action : create_unknown_expression();
                }

                branch = allocate(1, sizeof(conditional_branch_t));
                branch->condition = create_literal_expression(new_boolean(BOOLEAN_TRUE));
                branch->action = action;
                add_list_item(branches, branch);

                break;
            }
            else
            {
                destroy_string(keyword);

                break;
            }
        }
        else
        {
            break;
        }
    }

    return create_branch_expression(branches);
}

static expression_t *read_loop_expression(capsule_t *capsule)
{
    expression_t *condition, *action;

    condition = read_any_expression(capsule);

    if (!condition || condition->type == EXPRESSION_TYPE_UNKNOWN)
    {
        return condition != NULL ? condition : create_unknown_expression();
    }

    action = read_any_expression(capsule);

    if (!action || action->type == EXPRESSION_TYPE_UNKNOWN)
    {
        destroy_expression(condition);

        return action != NULL ? action : create_unknown_expression();
    }

    return create_loop_expression(condition, action);
}

static expression_t *read_catch_expression(capsule_t *capsule)
{
    expression_t *action;

    action = read_any_expression(capsule);

    if (!action || action->type == EXPRESSION_TYPE_UNKNOWN)
    {
        return action != NULL ? action : create_unknown_expression();
    }

    return create_catch_expression(action);
}

static expression_t *read_return_expression(capsule_t *capsule)
{
    expression_t *pick;

    pick = read_any_expression(capsule);

    if (!pick || pick->type == EXPRESSION_TYPE_UNKNOWN)
    {
        return pick != NULL ? pick : create_unknown_expression();
    }

    return create_return_expression(pick);
}

static expression_t *read_break_expression(capsule_t *capsule)
{
    expression_t *pick;

    pick = read_any_expression(capsule);

    if (!pick || pick->type == EXPRESSION_TYPE_UNKNOWN)
    {
        return pick != NULL ? pick : create_unknown_expression();
    }

    return create_break_expression(pick);
}

static expression_t *read_continue_expression(capsule_t *capsule)
{
    expression_t *pick;

    pick = read_any_expression(capsule);

    if (!pick || pick->type == EXPRESSION_TYPE_UNKNOWN)
    {
        return pick != NULL ? pick : create_unknown_expression();
    }

    return create_continue_expression(pick);
}

static expression_t *read_throw_expression(capsule_t *capsule)
{
    expression_t *error;

    error = read_any_expression(capsule);

    if (!error || error->type == EXPRESSION_TYPE_UNKNOWN)
    {
        return error != NULL ? error : create_unknown_expression();
    }

    return create_throw_expression(error);
}

static expression_t *read_snippet_expression(capsule_t *capsule)
{
    list_t *expressions;

    expressions = empty_list(destroy_expression_unsafe, 1);

    while (1)
    {
        expression_t *expression;
        token_t *optional;

        optional = peek_token(capsule);

        if (is_symbol_token('/', capsule->scanner.code, optional))
        {
            next_token(capsule);

            break;
        }

        expression = read_any_expression(capsule);

        if (!expression || expression->type == EXPRESSION_TYPE_UNKNOWN)
        {
            destroy_list(expressions);

            return expression != NULL ? expression : create_unknown_expression();
        }

        add_list_item(expressions, expression);
    }

    return create_snippet_expression(expressions);
}

static conditional_branch_t *read_conditional_branch(capsule_t *capsule)
{
    conditional_branch_t *branch;
    expression_t *condition, *action;

    condition = read_any_expression(capsule);

    if (!condition || condition->type == EXPRESSION_TYPE_UNKNOWN)
    {
        if (condition)
        {
            destroy_expression(condition);
        }

        return NULL;
    }

    action = read_any_expression(capsule);

    if (!action || action->type == EXPRESSION_TYPE_UNKNOWN)
    {
        destroy_expression(condition);

        if (action)
        {
            destroy_expression(action);
        }

        return NULL;
    }

    branch = allocate(1, sizeof(conditional_branch_t));
    branch->condition = condition;
    branch->action = action;

    return branch;
}

static token_t *peek_token(capsule_t *capsule)
{
    if (!capsule->ahead)
    {
        token_t *future;

        future = scan_token(&capsule->scanner);

        if (future)
        {
            capsule->future = (*future);
            capsule->hasFuture = 1;
        }
        else
        {
            capsule->hasFuture = 0;
        }

        capsule->ahead = 1;
    }

    return capsule->hasFuture ? &capsule->future : NULL;
}

static token_t *next_token(capsule_t *capsule)
{
    if (capsule->ahead)
    {
        if (capsule->hasFuture)
        {
            capsule->present = capsule->future;
        }

        capsule->hasPresent = capsule->hasFuture;
        capsule->ahead = 0;
    }
    else
    {
        token_t *present;

        present = scan_token(&capsule->scanner);

        if (present)
        {
            capsule->present = (*present);
            capsule->hasPresent = 1;
        }
        else
        {
            capsule->hasPresent = 0;
        }
    }

    return capsule->hasPresent ? &capsule->present : NULL;
}

static token_t *scan_token(scanner_t *scanner)
{
    while (scanner->state == SCANNER_STATE_RUNNING)
    {
        progress_scanner(scanner);

        if (scanner->state == SCANNER_STATE_RUNNING)
        {
            if (scanner->token.type == TOKEN_TYPE_WHITESPACE
                || scanner->token.type == TOKEN_TYPE_COMMENT)
            {
                continue;
            }

            return &scanner->token;
        }
    }

    return NULL;
}

static string_t *substring_using_token(const string_t *code, const token_t *token)
{
    char *bytes;
    size_t length;

    length = token->end - token->start;
    bytes = allocate(length, sizeof(char));
    memcpy(bytes, code->bytes + token->start, length);

    return create_string(bytes, length);
}

static string_t *substring_to_newline(const string_t *code, size_t start, size_t limit)
{
    char *bytes;
    size_t length;

    for (length = 0; length < limit; length++)
    {
        size_t offset;

        offset = start + length;

        if (offset >= code->length|| code->bytes[offset] == '\n')
        {
            break;
        }
    }

    bytes = allocate(length, sizeof(char));
    memcpy(bytes, code->bytes + start, length);

    return create_string(bytes, length);
}

static identifier_t *parse_identifier(const string_t *code, const token_t *token)
{
    identifier_t *identifier;
    identifier_type_t type;
    string_t *name;
    token_t offset;

    if (code->bytes[token->start] == '$')
    {
        type = IDENTIFIER_TYPE_VARIABLE;
    }
    else if (code->bytes[token->start] == '@')
    {
        type = IDENTIFIER_TYPE_FUNCTION;
    }
    else
    {
        crash_with_message("unsupported branch invoked");
        return NULL;
    }

    offset.start = token->start + 1;
    offset.end = token->end;

    if (code->bytes[offset.start] != '"')
    {
        name = substring_using_token(code, &offset);
    }
    else
    {
        name = unescape_string(code, &offset);

        if (!name)
        {
            return NULL;
        }
    }

    identifier = allocate(1, sizeof(identifier_t));
    identifier->type = type;
    identifier->name = name;

    return identifier;
}

static string_t *unescape_string(const string_t *code, const token_t *token)
{
    char *bytes;
    size_t length, escapeCount, index, placement;

    escapeCount = 0;

    for (index = token->start; index < token->end; index++)
    {
        if (code->bytes[index] == '\\')
        {
            escapeCount++;
            index++;

            if ((index + 4) < token->end && code->bytes[index] == 'a')
            {
                escapeCount += 3;
                index += 2;
            }
        }
    }

    length = token->end - token->start - 2 - escapeCount;

    if (length == 0)
    {
        return empty_string();
    }

    bytes = allocate(length, sizeof(char));

    for (index = token->start + 1, placement = 0; index < token->end - 1; index++)
    {
        if (code->bytes[index] != '\\')
        {
            bytes[placement++] = code->bytes[index];
        }
        else
        {
            switch (code->bytes[++index])
            {
                case 't':
                    bytes[placement++] = '\t';
                    break;

                case 'n':
                    bytes[placement++] = '\n';
                    break;

                case 'r':
                    bytes[placement++] = '\r';
                    break;

                case '"':
                    bytes[placement++] = '"';
                    break;

                case '\\':
                    bytes[placement++] = '\\';
                    break;

                case 'a':
                {
                    if ((token->end - index - 1) >= 3)
                    {
                        string_t string;
                        number_t number;
                        int integer;
                        char sequence[3];
                        size_t peek;

                        index += 1;

                        for (peek = index; (peek - index) < 3; peek++)
                        {
                            unsigned char symbol;

                            symbol = code->bytes[peek];

                            if (symbol < 48 || symbol > 57)
                            {
                                free(bytes);

                                return NULL;
                            }
                        }

                        memcpy(sequence, code->bytes + index, 3);
                        string.bytes = sequence;
                        string.length = 3;
                        integer = 0;

                        if (string_to_number(&string, &number) != 0
                            || number_to_integer(number, &integer) != 0
                            || integer < 0 || integer > 255)
                        {
                            free(bytes);

                            return NULL;
                        }

                        bytes[placement++] = (unsigned char) integer;
                        index += 2;
                    }
                    else
                    {
                        free(bytes);

                        return NULL;
                    }

                    break;
                }

                default:
                    free(bytes);
                    return NULL;
            }
        }
    }

    return create_string(bytes, placement);
}

static char is_symbol_token(char symbol, const string_t *code, const token_t *token)
{
    return token
        && token->type == TOKEN_TYPE_SYMBOL
        && code->bytes[token->start] == symbol;
}

static expression_t *create_unknown_expression()
{
    return create_expression(EXPRESSION_TYPE_UNKNOWN, NULL);
}

static expression_t *create_literal_expression(value_t *value)
{
    literal_expression_data_t *data;

    data = allocate(1, sizeof(literal_expression_data_t));
    data->value = value;

    return create_expression(EXPRESSION_TYPE_LITERAL, data);
}

static expression_t *create_reference_expression(identifier_t *identifier)
{
    reference_expression_data_t *data;

    data = allocate(1, sizeof(reference_expression_data_t));
    data->identifier = identifier;

    return create_expression(EXPRESSION_TYPE_REFERENCE, data);
}

static expression_t *create_assignment_expression(identifier_t *identifier, expression_t *value)
{
    assignment_expression_data_t *data;

    data = allocate(1, sizeof(assignment_expression_data_t));
    data->identifier = identifier;
    data->value = value;

    return create_expression(EXPRESSION_TYPE_ASSIGNMENT, data);
}

static expression_t *create_invoke_expression(identifier_t *identifier, list_t *arguments)
{
    invoke_expression_data_t *data;

    data = allocate(1, sizeof(invoke_expression_data_t));
    data->identifier = identifier;
    data->arguments = arguments;

    return create_expression(EXPRESSION_TYPE_INVOKE, data);
}

static expression_t *create_branch_expression(list_t *branches)
{
    branch_expression_data_t *data;

    data = allocate(1, sizeof(branch_expression_data_t));
    data->branches = branches;

    return create_expression(EXPRESSION_TYPE_BRANCH, data);
}

static expression_t *create_loop_expression(expression_t *condition, expression_t *action)
{
    loop_expression_data_t *data;

    data = allocate(1, sizeof(loop_expression_data_t));
    data->condition = condition;
    data->action = action;

    return create_expression(EXPRESSION_TYPE_LOOP, data);
}

static expression_t *create_catch_expression(expression_t *action)
{
    catch_expression_data_t *data;

    data = allocate(1, sizeof(catch_expression_data_t));
    data->action = action;

    return create_expression(EXPRESSION_TYPE_CATCH, data);
}

static expression_t *create_argument_expression()
{
    return create_expression(EXPRESSION_TYPE_ARGUMENT, NULL);
}

static expression_t *create_return_expression(expression_t *pick)
{
    return_expression_data_t *data;

    data = allocate(1, sizeof(return_expression_data_t));
    data->pick = pick;

    return create_expression(EXPRESSION_TYPE_RETURN, data);
}

static expression_t *create_break_expression(expression_t *pick)
{
    break_expression_data_t *data;

    data = allocate(1, sizeof(break_expression_data_t));
    data->pick = pick;

    return create_expression(EXPRESSION_TYPE_BREAK, data);
}

static expression_t *create_continue_expression(expression_t *pick)
{
    continue_expression_data_t *data;

    data = allocate(1, sizeof(continue_expression_data_t));
    data->pick = pick;

    return create_expression(EXPRESSION_TYPE_CONTINUE, data);
}

static expression_t *create_throw_expression(expression_t *error)
{
    throw_expression_data_t *data;

    data = allocate(1, sizeof(throw_expression_data_t));
    data->error = error;

    return create_expression(EXPRESSION_TYPE_THROW, data);
}

static expression_t *create_snippet_expression(list_t *expressions)
{
    snippet_expression_data_t *data;

    data = allocate(1, sizeof(snippet_expression_data_t));
    data->expressions = expressions;

    return create_expression(EXPRESSION_TYPE_SNIPPET, data);
}

static expression_t *create_expression(expression_type_t type, void *data)
{
    expression_t *expression;

    expression = allocate(1, sizeof(expression_t));
    expression->type = type;
    expression->data = data;

    return expression;
}

static void destroy_expression_unsafe(void *expression)
{
    destroy_expression((expression_t *) expression);
}

static void dereference_value_unsafe(void *value)
{
    dereference_value((value_t *) value);
}

static void destroy_conditional_branch(conditional_branch_t *branch)
{
    if (branch->condition)
    {
        destroy_expression(branch->condition);
    }

    if (branch->action)
    {
        destroy_expression(branch->action);
    }

    free(branch);
}

static void destroy_conditional_branch_unsafe(void *branch)
{
    destroy_conditional_branch((conditional_branch_t *) branch);
}
