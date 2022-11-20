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

static statement_t *read_any_statement(capsule_t *capsule);
static statement_t *read_list_statement(capsule_t *capsule);
static statement_t *read_map_statement(capsule_t *capsule);
static statement_t *read_assignment_statement(capsule_t *capsule, identifier_t *identifier);
static statement_t *read_invoke_statement(capsule_t *capsule, identifier_t *identifier);
static statement_t *read_branch_statement(capsule_t *capsule);
static statement_t *read_loop_statement(capsule_t *capsule);
static statement_t *read_catch_statement(capsule_t *capsule);
static statement_t *read_throw_statement(capsule_t *capsule);
static char is_value_statement(statement_t *statement);
static token_t *peek_token(capsule_t *capsule);
static token_t *next_token(capsule_t *capsule);
static token_t *scan_token(scanner_t *scanner);
static char *substring_using_token(char *code, token_t *token);
static char *substring_to_newline(char *code, size_t start, size_t limit);
static identifier_t *parse_identifier(char *code, token_t *token);
static char *unescape_string(char *code, token_t *token);
static char is_symbol_token(char symbol, char *code, token_t *token);
static statement_t *create_unknown_statement();
static statement_t *create_literal_statement(value_t *value);
static statement_t *create_assignment_statement(identifier_t *identifier, statement_t *value);
static statement_t *create_invoke_statement(identifier_t *identifier, list_t *arguments);
static statement_t *create_branch_statement(statement_t *condition, list_t *pass, list_t *fail);
static statement_t *create_loop_statement(statement_t *condition, list_t *body);
static statement_t *create_catch_statement(list_t *body);
static statement_t *create_throw_statement(statement_t *error);
static statement_t *create_reference_statement(identifier_t *identifier);
static statement_t *create_statement(statement_type_t type, void *data);
static void destroy_statement_unsafe(void *statement);
static void dereference_value_unsafe(void *value);

script_t *parse_script(char *code)
{
    capsule_t capsule;
    script_t *script;
    list_t *statements;
    char *errorMessage;
    char *hintMessage;

    capsule.hasPresent = 0;
    capsule.hasFuture = 0;
    capsule.ahead = 0;
    start_scanner(&capsule.scanner, code);

    statements = empty_list(destroy_statement_unsafe, 1);
    errorMessage = NULL;
    hintMessage = NULL;

    while (capsule.scanner.state == SCANNER_STATE_RUNNING)
    {
        statement_t *statement;

        statement = read_any_statement(&capsule);

        if (statement)
        {
            if (statement->type == STATEMENT_TYPE_UNKNOWN && capsule.scanner.state != SCANNER_STATE_ERRORED)
            {
                destroy_list(statements);
                destroy_statement(statement);
                statements = NULL;
                errorMessage = copy_string("failed to parse code");
                hintMessage = substring_to_newline(capsule.scanner.code, capsule.present.start, 80);
                break;
            }

            add_list_item(statements, statement);
        }
    }

    if (capsule.scanner.state == SCANNER_STATE_ERRORED)
    {
        errorMessage = copy_string("failed to lex code");
        hintMessage = substring_to_newline(capsule.scanner.code, capsule.scanner.token.start, 80);
    }

    script = allocate(sizeof(script_t));
    script->statements = statements;
    script->errorMessage = errorMessage;
    script->hintMessage = hintMessage;

    return script;
}

void destroy_script(script_t *script)
{
    if (script->errorMessage)
    {
        free(script->errorMessage);
    }

    if (script->hintMessage)
    {
        free(script->hintMessage);
    }

    if (script->statements)
    {
        destroy_list(script->statements);
    }

    free(script);
}

void destroy_statement(statement_t *statement)
{
    if (statement->data)
    {
        switch (statement->type)
        {
            case STATEMENT_TYPE_LITERAL:
            {
                literal_statement_data_t *data;

                data = statement->data;

                if (data->value)
                {
                    dereference_value(data->value);
                }

                free(data);
                break;
            }

            case STATEMENT_TYPE_ASSIGNMENT:
            {
                assignment_statement_data_t *data;

                data = statement->data;

                if (data->identifier)
                {
                    destroy_identifier(data->identifier);
                }

                if (data->value)
                {
                    destroy_statement(data->value);
                }

                free(data);
                break;
            }

            case STATEMENT_TYPE_INVOKE:
            {
                invoke_statement_data_t *data;

                data = statement->data;

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

            case STATEMENT_TYPE_BRANCH:
            {
                branch_statement_data_t *data;

                data = statement->data;

                if (data->condition)
                {
                    destroy_statement(data->condition);
                }

                if (data->pass)
                {
                    destroy_list(data->pass);
                }

                if (data->fail)
                {
                    destroy_list(data->fail);
                }

                free(data);
                break;
            }

            case STATEMENT_TYPE_LOOP:
            {
                loop_statement_data_t *data;

                data = statement->data;

                if (data->condition)
                {
                    destroy_statement(data->condition);
                }

                if (data->body)
                {
                    destroy_list(data->body);
                }

                free(data);
                break;
            }

            case STATEMENT_TYPE_CATCH:
            {
                catch_statement_data_t *data;

                data = statement->data;

                if (data->body)
                {
                    destroy_list(data->body);
                }

                free(data);
                break;
            }

            case STATEMENT_TYPE_THROW:
            {
                throw_statement_data_t *data;

                data = statement->data;

                if (data->error)
                {
                    destroy_statement(data->error);
                }

                free(data);
                break;
            }

            case STATEMENT_TYPE_REFERENCE:
            {
                reference_statement_data_t *data;

                data = statement->data;

                if (data->identifier)
                {
                    destroy_identifier(data->identifier);
                }

                free(data);
                break;
            }

            default:
                crash_with_message("unsupported branch invoked");
                break;
        }
    }

    free(statement);
}

void destroy_identifier(identifier_t *identifier)
{
    if (identifier->name)
    {
        free(identifier->name);
    }

    free(identifier);
}

static statement_t *read_any_statement(capsule_t *capsule)
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
        char *text;

        text = substring_using_token(capsule->scanner.code, token);

        if (string_to_number(text, &value) != 0)
        {
            free(text);

            return create_unknown_statement();
        }

        free(text);

        return create_literal_statement(new_number(value));
    }
    else if (token->type == TOKEN_TYPE_STRING)
    {
        char *value;

        value = unescape_string(capsule->scanner.code, token);

        return create_literal_statement(steal_string(value));
    }
    else if (token->type == TOKEN_TYPE_IDENTIFIER)
    {
        identifier_t *identifier;
        token_t *optional;

        identifier = parse_identifier(capsule->scanner.code, token);
        optional = peek_token(capsule);

        if (is_symbol_token('=', capsule->scanner.code, optional))
        {
            next_token(capsule);

            return read_assignment_statement(capsule, identifier);
        }
        else if (is_symbol_token('(', capsule->scanner.code, optional))
        {
            next_token(capsule);

            return read_invoke_statement(capsule, identifier);
        }
        else
        {
            if (identifier)
            {
                return create_reference_statement(identifier);
            }
            else
            {
                return create_unknown_statement();
            }
        }
    }
    else if (token->type == TOKEN_TYPE_KEYWORD)
    {
        statement_t *statement;
        char *keyword;

        keyword = substring_using_token(capsule->scanner.code, token);

        if (strcmp(keyword, "null") == 0)
        {
            statement = create_literal_statement(new_null());
        }
        else if (strcmp(keyword, "false") == 0)
        {
            statement = create_literal_statement(new_boolean(FALSE));
        }
        else if (strcmp(keyword, "true") == 0)
        {
            statement = create_literal_statement(new_boolean(TRUE));
        }
        else if (strcmp(keyword, "if") == 0)
        {
            statement = read_branch_statement(capsule);
        }
        else if (strcmp(keyword, "while") == 0)
        {
            statement = read_loop_statement(capsule);
        }
        else if (strcmp(keyword, "catch") == 0)
        {
            statement = read_catch_statement(capsule);
        }
        else if (strcmp(keyword, "throw") == 0)
        {
            statement = read_throw_statement(capsule);
        }
        else
        {
            crash_with_message("unsupported branch invoked");
            return NULL;
        }

        free(keyword);

        return statement;
    }
    else if (token->type == TOKEN_TYPE_SYMBOL)
    {
        if (is_symbol_token('[', capsule->scanner.code, token))
        {
            return read_list_statement(capsule);
        }
        else if (is_symbol_token('{', capsule->scanner.code, token))
        {
            return read_map_statement(capsule);
        }
    }

    return create_unknown_statement();
}

static statement_t *read_list_statement(capsule_t *capsule)
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

                return create_unknown_statement();
            }

            ready = 1;
        }
        else
        {
            statement_t *item;
            literal_statement_data_t *data;

            if (!ready)
            {
                destroy_list(items);

                return create_unknown_statement();
            }

            item = read_any_statement(capsule);

            if (!item || item->type != STATEMENT_TYPE_LITERAL)
            {
                if (item)
                {
                    destroy_statement(item);
                }

                destroy_list(items);

                return create_unknown_statement();
            }

            ready = 0;
            data = (literal_statement_data_t *) item->data;
            add_list_item(items, data->value);
            data->value = NULL;
            destroy_statement(item);
        }
    }

    return create_literal_statement(steal_list(items));
}

static statement_t *read_map_statement(capsule_t *capsule)
{
    map_t *mappings;
    int mode; /* 0:wants_key, 1:wants_glue, 2:wants_value, 3:wants_delimiter_or_end */
    char *hold;

    mappings = empty_map(hash_string, dereference_value_unsafe, 1);
    mode = 0;
    hold = NULL;

    while (1)
    {
        token_t *optional;

        optional = peek_token(capsule);

        if (mode == 0)
        {
            statement_t *key;
            literal_statement_data_t *data;
            value_t *steal;

            if (is_symbol_token('}', capsule->scanner.code, optional))
            {
                next_token(capsule);

                break;
            }

            key = read_any_statement(capsule);

            if (!key || key->type != STATEMENT_TYPE_LITERAL)
            {
                if (key)
                {
                    destroy_statement(key);
                }

                destroy_map(mappings);

                return create_unknown_statement();
            }

            mode = 1;
            data = key->data;
            steal = data->value;

            if (steal->type != VALUE_TYPE_STRING)
            {
                destroy_statement(key);
                destroy_map(mappings);

                return create_unknown_statement();
            }

            hold = view_string(steal);
            steal->data = NULL;
            destroy_statement(key);
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
                free(hold);
                destroy_map(mappings);

                return create_unknown_statement();
            }
        }
        else if (mode == 2)
        {
            statement_t *value;
            literal_statement_data_t *data;

            value = read_any_statement(capsule);

            if (!value || value->type != STATEMENT_TYPE_LITERAL)
            {
                if (value)
                {
                    destroy_statement(value);
                }

                free(hold);
                destroy_map(mappings);

                return create_unknown_statement();
            }

            mode = 3;
            data = value->data;
            set_map_item(mappings, hold, data->value);
            hold = NULL;
            data->value = NULL;
            destroy_statement(value);
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

    return create_literal_statement(steal_map(mappings));
}

static statement_t *read_assignment_statement(capsule_t *capsule, identifier_t *identifier)
{
    statement_t *value;

    value = read_any_statement(capsule);

    if (!value || !is_value_statement(value))
    {
        if (value)
        {
            destroy_statement(value);
        }

        destroy_identifier(identifier);

        return create_unknown_statement();
    }

    if (identifier && value)
    {
        return create_assignment_statement(identifier, value);
    }
    else
    {
        return create_unknown_statement();
    }
}

static statement_t *read_invoke_statement(capsule_t *capsule, identifier_t *identifier)
{
    list_t *arguments;
    int ready;

    arguments = empty_list(destroy_statement_unsafe, 1);
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

                return create_unknown_statement();
            }

            ready = 1;
        }
        else
        {
            statement_t *argument;

            if (!ready)
            {
                destroy_identifier(identifier);
                destroy_list(arguments);

                return create_unknown_statement();
            }

            argument = read_any_statement(capsule);

            if (!argument || !is_value_statement(argument))
            {
                if (argument)
                {
                    destroy_statement(argument);
                }

                destroy_identifier(identifier);
                destroy_list(arguments);

                return create_unknown_statement();
            }

            ready = 0;
            add_list_item(arguments, argument);
        }
    }

    if (identifier)
    {
        return create_invoke_statement(identifier, arguments);
    }
    else
    {
        destroy_identifier(identifier);
        destroy_list(arguments);

        return create_unknown_statement();
    }
}

static statement_t *read_branch_statement(capsule_t *capsule)
{
    statement_t *condition;
    list_t *pass, *fail;
    token_t *optional;

    condition = read_any_statement(capsule);

    if (!condition || !is_value_statement(condition))
    {
        if (condition)
        {
            destroy_statement(condition);
        }

        return create_unknown_statement();
    }

    optional = peek_token(capsule);

    if (!is_symbol_token('{', capsule->scanner.code, optional))
    {
        destroy_statement(condition);

        return create_unknown_statement();
    }

    next_token(capsule);
    pass = empty_list(destroy_statement_unsafe, 1);
    fail = NULL;

    while (1)
    {
        statement_t *part;

        optional = peek_token(capsule);

        if (is_symbol_token('}', capsule->scanner.code, optional))
        {
            next_token(capsule);

            break;
        }

        part = read_any_statement(capsule);

        if (!part || part->type == STATEMENT_TYPE_UNKNOWN)
        {
            destroy_statement(condition);
            destroy_list(pass);

            return create_unknown_statement();
        }

        add_list_item(pass, part);
    }

    optional = peek_token(capsule);

    if (optional && optional->type == TOKEN_TYPE_KEYWORD)
    {
        char *keyword;

        keyword = substring_using_token(capsule->scanner.code, optional);

        if (strcmp(keyword, "else") == 0)
        {
            free(keyword);
            next_token(capsule);
            optional = peek_token(capsule);

            if (!is_symbol_token('{', capsule->scanner.code, optional))
            {
                destroy_statement(condition);
                destroy_list(pass);

                return create_unknown_statement();
            }

            next_token(capsule);
            fail = empty_list(destroy_statement_unsafe, 1);

            while (1)
            {
                statement_t *part;

                optional = peek_token(capsule);

                if (is_symbol_token('}', capsule->scanner.code, optional))
                {
                    next_token(capsule);

                    break;
                }

                part = read_any_statement(capsule);

                if (!part || part->type == STATEMENT_TYPE_UNKNOWN)
                {
                    destroy_statement(condition);
                    destroy_list(pass);
                    destroy_list(fail);

                    return create_unknown_statement();
                }

                add_list_item(fail, part);
            }
        }
        else
        {
            free(keyword);
        }
    }

    return create_branch_statement(condition, pass, fail);
}

static statement_t *read_loop_statement(capsule_t *capsule)
{
    statement_t *condition;
    list_t *body;
    token_t *optional;

    condition = read_any_statement(capsule);

    if (!condition || !is_value_statement(condition))
    {
        if (condition)
        {
            destroy_statement(condition);
        }

        return create_unknown_statement();
    }

    optional = peek_token(capsule);

    if (!is_symbol_token('{', capsule->scanner.code, optional))
    {
        destroy_statement(condition);

        return create_unknown_statement();
    }

    next_token(capsule);
    body = empty_list(destroy_statement_unsafe, 1);

    while (1)
    {
        statement_t *part;

        optional = peek_token(capsule);

        if (is_symbol_token('}', capsule->scanner.code, optional))
        {
            next_token(capsule);

            break;
        }

        part = read_any_statement(capsule);

        if (!part || part->type == STATEMENT_TYPE_UNKNOWN)
        {
            destroy_statement(condition);
            destroy_list(body);

            return create_unknown_statement();
        }

        add_list_item(body, part);
    }

    return create_loop_statement(condition, body);
}

static statement_t *read_catch_statement(capsule_t *capsule)
{
    list_t *body;
    token_t *optional;

    optional = peek_token(capsule);

    if (!is_symbol_token('{', capsule->scanner.code, optional))
    {
        return create_unknown_statement();
    }

    next_token(capsule);
    body = empty_list(destroy_statement_unsafe, 1);

    while (1)
    {
        statement_t *part;

        optional = peek_token(capsule);

        if (is_symbol_token('}', capsule->scanner.code, optional))
        {
            next_token(capsule);

            break;
        }

        part = read_any_statement(capsule);

        if (!part || part->type == STATEMENT_TYPE_UNKNOWN)
        {
            destroy_list(body);

            return create_unknown_statement();
        }

        add_list_item(body, part);
    }

    return create_catch_statement(body);
}

static statement_t *read_throw_statement(capsule_t *capsule)
{
    statement_t *error;

    error = read_any_statement(capsule);

    if (!error || !is_value_statement(error))
    {
        if (error)
        {
            destroy_statement(error);
        }

        return create_unknown_statement();
    }

    return create_throw_statement(error);
}

static char is_value_statement(statement_t *statement)
{
    switch (statement->type)
    {
        case STATEMENT_TYPE_LITERAL:
        case STATEMENT_TYPE_REFERENCE:
        case STATEMENT_TYPE_INVOKE:
        case STATEMENT_TYPE_BRANCH:
        case STATEMENT_TYPE_CATCH:
            return 1;

        default:
            return 0;
    }
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

static char *substring_using_token(char *code, token_t *token)
{
    char *text;
    size_t textLength;

    textLength = token->end - token->start;
    text = allocate(sizeof(char) * (textLength + 1));
    memcpy(text, code + token->start, textLength);
    text[textLength] = '\0';

    return text;
}

static char *substring_to_newline(char *code, size_t start, size_t limit)
{
    char *line;
    size_t lineLength, codeLength;

    codeLength = strlen(code);

    for (lineLength = 0; lineLength < limit; lineLength++)
    {
        size_t offset;

        offset = start + lineLength;

        if (offset >= codeLength|| code[offset] == '\n')
        {
            break;
        }
    }

    line = allocate(sizeof(char) * (lineLength + 1));
    memcpy(line, code + start, lineLength);
    line[lineLength] = '\0';

    return line;
}

static identifier_t *parse_identifier(char *code, token_t *token)
{
    identifier_t *identifier;
    identifier_type_t type;
    char *name;
    token_t offset;

    if (code[token->start] == '$')
    {
        type = IDENTIFIER_TYPE_VARIABLE;
    }
    else if (code[token->start] == '@')
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

    if (code[offset.start] != '"')
    {
        name = substring_using_token(code, &offset);
    }
    else
    {
        name = unescape_string(code, &offset);
    }

    identifier = allocate(sizeof(identifier_t));
    identifier->type = type;
    identifier->name = name;

    return identifier;
}

static char *unescape_string(char *code, token_t *token)
{
    char *text;
    size_t textLength, escapeCount, index, placement;

    escapeCount = 0;

    for (index = token->start; index < token->end; index++)
    {
        if (code[index] == '\\')
        {
            escapeCount++;
            index++;
        }
    }

    textLength = token->end - token->start - 2 - escapeCount;
    text = allocate(sizeof(char) * (textLength + 1));

    for (index = token->start + 1, placement = 0; index < token->end - 1; index++)
    {
        if (code[index] != '\\')
        {
            text[placement++] = code[index];
        }
        else
        {
            switch (code[++index])
            {
                case 't':
                    text[placement++] = '\t';
                    break;

                case 'n':
                    text[placement++] = '\n';
                    break;

                case 'r':
                    text[placement++] = '\r';
                    break;

                case '"':
                    text[placement++] = '"';
                    break;

                case '\\':
                    text[placement++] = '\\';
                    break;

                default:
                    break;
            }
        }
    }

    text[textLength] = '\0';

    return text;
}

static char is_symbol_token(char symbol, char *code, token_t *token)
{
    return token
        && token->type == TOKEN_TYPE_SYMBOL
        && code[token->start] == symbol;
}

static statement_t *create_unknown_statement()
{
    return create_statement(STATEMENT_TYPE_UNKNOWN, NULL);
}

static statement_t *create_literal_statement(value_t *value)
{
    literal_statement_data_t *data;

    data = allocate(sizeof(literal_statement_data_t));
    data->value = value;

    return create_statement(STATEMENT_TYPE_LITERAL, data);
}

static statement_t *create_assignment_statement(identifier_t *identifier, statement_t *value)
{
    assignment_statement_data_t *data;

    data = allocate(sizeof(assignment_statement_data_t));
    data->identifier = identifier;
    data->value = value;

    return create_statement(STATEMENT_TYPE_ASSIGNMENT, data);
}

static statement_t *create_invoke_statement(identifier_t *identifier, list_t *arguments)
{
    invoke_statement_data_t *data;

    data = allocate(sizeof(invoke_statement_data_t));
    data->identifier = identifier;
    data->arguments = arguments;

    return create_statement(STATEMENT_TYPE_INVOKE, data);
}

static statement_t *create_branch_statement(statement_t *condition, list_t *pass, list_t *fail)
{
    branch_statement_data_t *data;

    data = allocate(sizeof(branch_statement_data_t));
    data->condition = condition;
    data->pass = pass;
    data->fail = fail;

    return create_statement(STATEMENT_TYPE_BRANCH, data);
}

static statement_t *create_loop_statement(statement_t *condition, list_t *body)
{
    loop_statement_data_t *data;

    data = allocate(sizeof(loop_statement_data_t));
    data->condition = condition;
    data->body = body;

    return create_statement(STATEMENT_TYPE_LOOP, data);
}

static statement_t *create_catch_statement(list_t *body)
{
    catch_statement_data_t *data;

    data = allocate(sizeof(catch_statement_data_t));
    data->body = body;

    return create_statement(STATEMENT_TYPE_CATCH, data);
}

static statement_t *create_throw_statement(statement_t *error)
{
    throw_statement_data_t *data;

    data = allocate(sizeof(throw_statement_data_t));
    data->error = error;

    return create_statement(STATEMENT_TYPE_THROW, data);
}

static statement_t *create_reference_statement(identifier_t *identifier)
{
    reference_statement_data_t *data;

    data = allocate(sizeof(reference_statement_data_t));
    data->identifier = identifier;

    return create_statement(STATEMENT_TYPE_REFERENCE, data);
}

static statement_t *create_statement(statement_type_t type, void *data)
{
    statement_t *statement;

    statement = allocate(sizeof(statement_t));
    statement->type = type;
    statement->data = data;

    return statement;
}

static void destroy_statement_unsafe(void *statement)
{
    destroy_statement((statement_t *) statement);
}

static void dereference_value_unsafe(void *value)
{
    dereference_value((value_t *) value);
}
