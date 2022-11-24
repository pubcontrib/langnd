#include <string.h>
#include "lex.h"
#include "utility.h"

static void read_whitespace_token(scanner_t *scanner);
static void read_comment_token(scanner_t *scanner);
static void read_number_token(scanner_t *scanner);
static void read_string_token(scanner_t *scanner);
static void read_identifier_token(scanner_t *scanner);
static void read_keyword_token(scanner_t *scanner);
static int has_another_symbol(scanner_t *scanner);
static char read_next_symbol(scanner_t *scanner);
static char peek_next_symbol(scanner_t *scanner);
static int is_whitespace_symbol(char symbol);
static int is_number_symbol(char symbol);
static int is_string_symbol(char symbol);
static int is_letter_symbol(char symbol);
static int is_short_identifier_symbol(char symbol);
static int is_keyword_symbol(char symbol);

void start_scanner(scanner_t *scanner, string_t *code)
{
    scanner->code = code;
    scanner->state = SCANNER_STATE_RUNNING;
    scanner->token.start = 0;
    scanner->token.end = 0;
    scanner->token.type = TOKEN_TYPE_UNKNOWN;
}

void progress_scanner(scanner_t *scanner)
{
    if (scanner->state != SCANNER_STATE_RUNNING)
    {
        return;
    }

    scanner->token.start = scanner->token.end;
    scanner->token.type = TOKEN_TYPE_UNKNOWN;

    while (has_another_symbol(scanner))
    {
        char symbol;

        symbol = read_next_symbol(scanner);

        if (is_whitespace_symbol(symbol))
        {
            read_whitespace_token(scanner);
            return;
        }
        else if (symbol == '#')
        {
            read_comment_token(scanner);
            return;
        }
        else if (is_number_symbol(symbol)
            || (symbol == '-' && has_another_symbol(scanner) && is_number_symbol(peek_next_symbol(scanner))))
        {
            read_number_token(scanner);
            return;
        }
        else if (symbol == '"')
        {
            read_string_token(scanner);
            return;
        }
        else if (symbol == '$' || symbol == '@')
        {
            read_identifier_token(scanner);
            return;
        }
        else if (is_keyword_symbol(symbol))
        {
            read_keyword_token(scanner);
            return;
        }
        else if (symbol == '=' || symbol == '(' || symbol == ')' || symbol == ',' || symbol == '{' || symbol == '}' || symbol == '[' || symbol == ']' || symbol == ':')
        {
            scanner->token.type = TOKEN_TYPE_SYMBOL;
            return;
        }
        else
        {
            scanner->state = SCANNER_STATE_ERRORED;
            return;
        }
    }

    scanner->state = SCANNER_STATE_COMPLETED;
}

static void read_whitespace_token(scanner_t *scanner)
{
    scanner->token.type = TOKEN_TYPE_WHITESPACE;

    while (has_another_symbol(scanner))
    {
        char symbol;

        symbol = peek_next_symbol(scanner);

        if (!is_whitespace_symbol(symbol))
        {
            return;
        }
        else
        {
            scanner->token.end++;
        }
    }
}

static void read_comment_token(scanner_t *scanner)
{
    scanner->token.type = TOKEN_TYPE_COMMENT;

    while (has_another_symbol(scanner))
    {
        char symbol;

        symbol = peek_next_symbol(scanner);

        if (symbol == '\n')
        {
            return;
        }
        else
        {
            scanner->token.end++;
        }
    }
}

static void read_number_token(scanner_t *scanner)
{
    int decimal;

    scanner->token.type = TOKEN_TYPE_NUMBER;
    decimal = 0;

    while (has_another_symbol(scanner))
    {
        char symbol;

        symbol = peek_next_symbol(scanner);

        if (symbol == '.')
        {
            if (!decimal)
            {
                scanner->token.end++;
                decimal = 1;
            }
            else
            {
                scanner->state = SCANNER_STATE_ERRORED;
                return;
            }
        }
        else if (is_number_symbol(symbol))
        {
            scanner->token.end++;
        }
        else
        {
            break;
        }
    }
}

static void read_string_token(scanner_t *scanner)
{
    int escaping, completed;

    scanner->token.type = TOKEN_TYPE_STRING;
    escaping = 0;
    completed = 0;

    while (has_another_symbol(scanner))
    {
        char symbol;

        symbol = peek_next_symbol(scanner);

        if (symbol == '"')
        {
            read_next_symbol(scanner);

            if (escaping)
            {
                escaping = 0;
            }
            else
            {
                completed = 1;
                break;
            }
        }
        else if (symbol == '\\')
        {
            read_next_symbol(scanner);
            escaping = !escaping;
        }
        else if (is_string_symbol(symbol))
        {
            read_next_symbol(scanner);

            if (escaping)
            {
                escaping = 0;
            }
        }
        else
        {
            break;
        }
    }

    if (!completed)
    {
        scanner->state = SCANNER_STATE_ERRORED;
        return;
    }
}

static void read_identifier_token(scanner_t *scanner)
{
    char symbol;

    scanner->token.type = TOKEN_TYPE_IDENTIFIER;

    if (!has_another_symbol(scanner))
    {
        scanner->state = SCANNER_STATE_ERRORED;
        return;
    }

    symbol = peek_next_symbol(scanner);

    if (is_short_identifier_symbol(symbol))
    {
        while (has_another_symbol(scanner))
        {
            symbol = peek_next_symbol(scanner);

            if (is_short_identifier_symbol(symbol))
            {
                scanner->token.end++;
            }
            else
            {
                break;
            }
        }
    }
    else if (symbol == '"')
    {
        read_next_symbol(scanner);
        read_string_token(scanner);
        scanner->token.type = TOKEN_TYPE_IDENTIFIER;
        return;
    }
    else
    {
        scanner->state = SCANNER_STATE_ERRORED;
        return;
    }
}

static void read_keyword_token(scanner_t *scanner)
{
    static const char *keywords[] = { "null", "false", "true", "if", "else", "while", "catch", "throw" };
    static const size_t keywordsLength = 8;
    size_t keywordsIndex;

    scanner->token.type = TOKEN_TYPE_KEYWORD;

    for (keywordsIndex = 0; keywordsIndex < keywordsLength; keywordsIndex++)
    {
        const char *keyword;
        int match;
        size_t keywordIndex, keywordLength;

        keyword = keywords[keywordsIndex];
        keywordLength = strlen(keyword);
        match = 1;

        for (keywordIndex = 0; keywordIndex < keywordLength; keywordIndex++)
        {
            size_t codeIndex;

            codeIndex = scanner->token.start + keywordIndex;

            if (codeIndex >= scanner->code->length)
            {
                scanner->state = SCANNER_STATE_ERRORED;
                return;
            }

            if (scanner->code->bytes[codeIndex] != keyword[keywordIndex])
            {
                match = 0;
                break;
            }
        }

        if (match)
        {
            scanner->token.end = scanner->token.start + keywordLength;
            return;
        }
    }

    scanner->state = SCANNER_STATE_ERRORED;
}

static int has_another_symbol(scanner_t *scanner)
{
    return scanner->token.end < scanner->code->length;
}

static char read_next_symbol(scanner_t *scanner)
{
    return scanner->code->bytes[scanner->token.end++];
}

static char peek_next_symbol(scanner_t *scanner)
{
    return scanner->code->bytes[scanner->token.end];
}

static int is_whitespace_symbol(char symbol)
{
    return symbol == ' ' || symbol == '\t' || symbol == '\n' || symbol == '\r';
}

static int is_number_symbol(char symbol)
{
    return symbol >= '0' && symbol <= '9';
}

static int is_string_symbol(char symbol)
{
    return symbol >= ' ' && symbol <= '~';
}

static int is_letter_symbol(char symbol)
{
    return symbol >= 'A' && symbol <= 'z';
}

static int is_short_identifier_symbol(char symbol)
{
    return is_letter_symbol(symbol)
        || is_number_symbol(symbol)
        || symbol == '_';
}

static int is_keyword_symbol(char symbol)
{
    return symbol >= 'a' && symbol <= 'z';
}
