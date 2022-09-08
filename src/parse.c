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
static statement_t *read_assignment_expression(capsule_t *capsule, identifier_t *identifier);
static statement_t *read_invoke_expression(capsule_t *capsule, identifier_t *identifier);
static char is_value_statement(statement_t *statement);
static char is_literal_statement(statement_t *statement);
static token_t *peek_token(capsule_t *capsule);
static token_t *next_token(capsule_t *capsule);
static token_t *scan_token(scanner_t *scanner);
static identifier_t *parse_identifier(token_t *token, char *code);
static char *unescape_string(char *code, size_t start, size_t end);
static void destroy_statement_unsafe(void *statement);

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

    statements = empty_list(destroy_statement_unsafe);
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
                break;
            }

            add_list_item(statements, statement);
        }
    }

    if (capsule.scanner.state == SCANNER_STATE_ERRORED)
    {
        char *hint;
        size_t hintLength;

        for (hintLength = 0; hintLength < 80; hintLength++)
        {
            size_t offset;

            offset = capsule.scanner.token.start + hintLength;

            if (offset >= capsule.scanner.codeLength
                || capsule.scanner.code[offset] == '\n')
            {
                break;
            }
        }

        hint = allocate(sizeof(char) * (hintLength + 1));
        memcpy(hint, capsule.scanner.code + capsule.scanner.token.start, hintLength);
        hint[hintLength] = '\0';

        errorMessage = copy_string("failed to lex code");
        hintMessage = hint;
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
            case STATEMENT_TYPE_BOOLEAN:
            {
                boolean_statement_data_t *data;

                data = statement->data;

                free(data);
                break;
            }

            case STATEMENT_TYPE_NUMBER:
            {
                number_statement_data_t *data;

                data = statement->data;

                free(data);
                break;
            }

            case STATEMENT_TYPE_STRING:
            {
                string_statement_data_t *data;

                data = statement->data;

                free(data->value);
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
                crash_with_message("unsupported branch PARSE_DESTROY_STATEMENT");
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
    statement_t *error;

    token = next_token(capsule);

    if (!token)
    {
        return NULL;
    }

    if (token->type == TOKEN_TYPE_NUMBER)
    {
        statement_t *statement;
        number_statement_data_t *data;
        number_t value;
        char *text;
        size_t textLength;

        textLength = token->end - token->start;
        text = allocate(sizeof(char) * (textLength + 1));
        memcpy(text, capsule->scanner.code + token->start, textLength);
        text[textLength] = '\0';
        string_to_number(text, &value);
        free(text);

        data = allocate(sizeof(number_statement_data_t));
        data->value = value;

        statement = allocate(sizeof(statement_t));
        statement->type = STATEMENT_TYPE_NUMBER;
        statement->data = data;

        return statement;
    }
    else if (token->type == TOKEN_TYPE_STRING)
    {
        statement_t *statement;
        string_statement_data_t *data;

        data = allocate(sizeof(string_statement_data_t));
        data->value = unescape_string(capsule->scanner.code, token->start, token->end);

        statement = allocate(sizeof(statement_t));
        statement->type = STATEMENT_TYPE_STRING;
        statement->data = data;

        return statement;
    }
    else if (token->type == TOKEN_TYPE_IDENTIFIER)
    {
        token_t *optional;

        optional = peek_token(capsule);

        if (optional
            && optional->type == TOKEN_TYPE_SYMBOL
            && capsule->scanner.code[optional->start] == '=')
        {
            identifier_t *identifier;

            identifier = parse_identifier(token, capsule->scanner.code);
            next_token(capsule);

            return read_assignment_expression(capsule, identifier);
        }
        else if (optional
            && optional->type == TOKEN_TYPE_SYMBOL
            && capsule->scanner.code[optional->start] == '(')
        {
            identifier_t *identifier;

            identifier = parse_identifier(token, capsule->scanner.code);
            next_token(capsule);

            return read_invoke_expression(capsule, identifier);
        }
        else
        {
            statement_t *statement;
            reference_statement_data_t *data;

            data = allocate(sizeof(reference_statement_data_t));
            data->identifier = parse_identifier(token, capsule->scanner.code);

            statement = allocate(sizeof(statement_t));

            if (data->identifier)
            {
                statement->type = STATEMENT_TYPE_REFERENCE;
                statement->data = data;
            }
            else
            {
                statement->type = STATEMENT_TYPE_UNKNOWN;
                statement->data = NULL;
            }

            return statement;
        }
    }
    else if (token->type == TOKEN_TYPE_KEYWORD)
    {
        statement_t *statement;
        char *keyword;
        size_t length;

        length = token->end - token->start;
        keyword = allocate(sizeof(char) * (length + 1));
        memcpy(keyword, capsule->scanner.code + token->start, length);
        keyword[length] = '\0';
        statement = allocate(sizeof(statement_t));

        if (strcmp(keyword, "null") == 0)
        {
            statement->type = STATEMENT_TYPE_NULL;
            statement->data = NULL;
        }
        else if (strcmp(keyword, "false") == 0)
        {
            boolean_statement_data_t *data;

            data = allocate(sizeof(boolean_statement_data_t));
            data->value = FALSE;
            statement->type = STATEMENT_TYPE_BOOLEAN;
            statement->data = data;
        }
        else if (strcmp(keyword, "true") == 0)
        {
            boolean_statement_data_t *data;

            data = allocate(sizeof(boolean_statement_data_t));
            data->value = TRUE;
            statement->type = STATEMENT_TYPE_BOOLEAN;
            statement->data = data;
        }
        else
        {
            crash_with_message("unsupported branch PARSE_KEYWORD_TOKEN");
        }

        free(keyword);

        return statement;
    }

    error = allocate(sizeof(statement_t));
    error->type = STATEMENT_TYPE_UNKNOWN;
    error->data = NULL;

    return error;
}

static statement_t *read_assignment_expression(capsule_t *capsule, identifier_t *identifier)
{
    statement_t *statement;
    assignment_statement_data_t *data;

    data = allocate(sizeof(assignment_statement_data_t));
    data->identifier = identifier;
    data->value = read_any_statement(capsule);

    statement = allocate(sizeof(statement_t));

    if (data->identifier && data->value)
    {
        statement->type = STATEMENT_TYPE_ASSIGNMENT;
        statement->data = data;
    }
    else
    {
        statement->type = STATEMENT_TYPE_UNKNOWN;
        statement->data = NULL;
    }

    return statement;
}

static statement_t *read_invoke_expression(capsule_t *capsule, identifier_t *identifier)
{
    statement_t *statement;
    invoke_statement_data_t *data;
    list_t *arguments;
    char ready;

    arguments = empty_list(destroy_statement_unsafe);
    ready = 1;

    while (1)
    {
        token_t *optional;

        optional = peek_token(capsule);

        if (optional
            && optional->type == TOKEN_TYPE_SYMBOL
            && capsule->scanner.code[optional->start] == ')')
        {
            next_token(capsule);
            break;
        }
        else if (optional
            && optional->type == TOKEN_TYPE_SYMBOL
            && capsule->scanner.code[optional->start] == ',')
        {
            next_token(capsule);

            if (ready)
            {
                destroy_identifier(identifier);
                destroy_list(arguments);
                statement = allocate(sizeof(statement_t));
                statement->type = STATEMENT_TYPE_UNKNOWN;
                statement->data = NULL;
                return statement;
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
                statement = allocate(sizeof(statement_t));
                statement->type = STATEMENT_TYPE_UNKNOWN;
                statement->data = NULL;
                return statement;
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
                statement = allocate(sizeof(statement_t));
                statement->type = STATEMENT_TYPE_UNKNOWN;
                statement->data = NULL;
                return statement;
            }

            ready = 0;
            add_list_item(arguments, argument);
        }
    }

    data = allocate(sizeof(invoke_statement_data_t));
    data->identifier = identifier;
    data->arguments = arguments;

    statement = allocate(sizeof(statement_t));

    if (data->identifier)
    {
        statement->type = STATEMENT_TYPE_INVOKE;
        statement->data = data;
    }
    else
    {
        destroy_identifier(identifier);
        destroy_list(arguments);
        statement->type = STATEMENT_TYPE_UNKNOWN;
        statement->data = NULL;
    }

    return statement;
}

static char is_value_statement(statement_t *statement)
{
    if (is_literal_statement(statement))
    {
        return 1;
    }

    switch (statement->type)
    {
        case STATEMENT_TYPE_REFERENCE:
        case STATEMENT_TYPE_INVOKE:
            return 1;

        default:
            return 0;
    }
}

static char is_literal_statement(statement_t *statement)
{
    switch (statement->type)
    {
        case STATEMENT_TYPE_NULL:
        case STATEMENT_TYPE_BOOLEAN:
        case STATEMENT_TYPE_NUMBER:
        case STATEMENT_TYPE_STRING:
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
        capsule->present = capsule->future;
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

static identifier_t *parse_identifier(token_t *token, char *code)
{
    identifier_t *identifier;
    identifier_type_t type;
    char *name;

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
        crash_with_message("unsupported branch PARSE_IDENTIFIER_PREFIX");
        return NULL;
    }

    if (code[token->start + 1] != '"')
    {
        size_t nameLength;

        nameLength = token->end - token->start - 1;
        name = allocate(sizeof(char) * (nameLength + 1));
        memcpy(name, code + token->start + 1, nameLength);
        name[nameLength] = '\0';
    }
    else
    {
        name = unescape_string(code, token->start + 1, token->end);
    }

    identifier = allocate(sizeof(identifier_t));
    identifier->type = type;
    identifier->name = name;

    return identifier;
}

static char *unescape_string(char *code, size_t start, size_t end)
{
    char *text;
    size_t textLength, escapeCount, index, placement;

    escapeCount = 0;

    for (index = start; index < end; index++)
    {
        if (code[index] == '\\')
        {
            escapeCount++;
            index++;
        }
    }

    textLength = end - start - 2 - escapeCount;
    text = allocate(sizeof(char) * (textLength + 1));

    for (index = start + 1, placement = 0; index < end - 1; index++)
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

static void destroy_statement_unsafe(void *statement)
{
    destroy_statement((statement_t *) statement);
}
