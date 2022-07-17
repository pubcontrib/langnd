#include <string.h>
#include "lex.h"
#include "utility.h"

static void read_whitespace_token(scanner_t *scanner);
static void read_comment_token(scanner_t *scanner);
static void read_number_token(scanner_t *scanner);
static void read_identifier_token(scanner_t *scanner);
static int has_another_symbol(scanner_t *scanner);
static char read_next_symbol(scanner_t *scanner);
static char peek_next_symbol(scanner_t *scanner);
static int is_whitespace_symbol(char symbol);
static int is_number_symbol(char symbol);
static int is_letter_symbol(char symbol);
static int is_short_identifier_symbol(char symbol);

void start_scanner(scanner_t *scanner, char *code)
{
    scanner->code = code;
    scanner->codeLength = strlen(code);
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
        else if (symbol == '$' || symbol == '@')
        {
            read_identifier_token(scanner);
            return;
        }
        else if (symbol == '=' || symbol == '(' || symbol == ')' || symbol == ',')
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
    else
    {
        scanner->state = SCANNER_STATE_ERRORED;
        return;
    }
}

static int has_another_symbol(scanner_t *scanner)
{
    return scanner->token.end < scanner->codeLength;
}

static char read_next_symbol(scanner_t *scanner)
{
    return scanner->code[scanner->token.end++];
}

static char peek_next_symbol(scanner_t *scanner)
{
    return scanner->code[scanner->token.end];
}

static int is_whitespace_symbol(char symbol)
{
    return symbol == ' ' || symbol == '\t' || symbol == '\n' || symbol == '\r';
}

static int is_number_symbol(char symbol)
{
    return symbol >= '0' && symbol <= '9';
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
