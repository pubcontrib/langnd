#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "execute.h"
#include "utility.h"

static int run_text(char *text);
static int run_help();
static int run_version();

typedef enum
{
    PROGRAM_MODE_UNKNOWN,
    PROGRAM_MODE_TEXT,
    PROGRAM_MODE_HELP,
    PROGRAM_MODE_VERSION
} program_mode_t;

int main(int argumentsCount, char **arguments)
{
    program_mode_t mode;
    int argumentsIndex;

    mode = PROGRAM_MODE_UNKNOWN;

    for (argumentsIndex = 1; argumentsIndex < argumentsCount; argumentsIndex++)
    {
        char *argument;
        size_t argumentLength;
        int argumentIndex;

        argument = arguments[argumentsIndex];
        argumentLength = strlen(argument);

        if (argumentLength == 0 || argument[0] != '-')
        {
            break;
        }

        if (strcmp(argument, "--") == 0)
        {
            argumentsIndex++;
            break;
        }

        for (argumentIndex = 1; argumentIndex < argumentLength; argumentIndex++)
        {
            char argumentShorthand;

            argumentShorthand = argument[argumentIndex];

            switch (argumentShorthand)
            {
                case 't':
                    mode = PROGRAM_MODE_TEXT;
                    break;

                case 'h':
                    mode = PROGRAM_MODE_HELP;
                    break;

                case 'v':
                    mode = PROGRAM_MODE_VERSION;
                    break;

                default:
                    crash_with_message("illegal option %c", argumentShorthand);
                    break;
            }
        }
    }

    switch (mode)
    {
        case PROGRAM_MODE_TEXT:
        {
            if (argumentsIndex < argumentsCount)
            {
                return run_text(arguments[argumentsIndex++]);
            }
            else
            {
                crash_with_message("missing required script argument");
                break;
            }
        }

        case PROGRAM_MODE_HELP:
            return run_help();

        case PROGRAM_MODE_VERSION:
            return run_version();

        default:
            break;
    }

    return PROGRAM_SUCCESS;
}

static int run_text(char *text)
{
    outcome_t *outcome;

    assure_portable_environment();

    outcome = execute(text);

    if (outcome->errorMessage)
    {
        fprintf(stderr, "%s: ", PROGRAM_NAME);

        if (outcome->errorMessage)
        {
            fprintf(stderr, "%.72s\n", outcome->errorMessage);
        }
        else
        {
            crash_with_message("unsupported branch CLI_ERROR_MESSAGE");
        }

        if (outcome->hintMessage)
        {
            fprintf(stderr, "    [hint] %.69s\n", outcome->hintMessage);
        }

        destroy_outcome(outcome);

        return PROGRAM_FAILURE;
    }

    destroy_outcome(outcome);

    return PROGRAM_SUCCESS;
}

static int run_help()
{
    printf("Usage:\n");
    printf("  %s -t script\n", PROGRAM_NAME);
    printf("\n");
    printf("Options:\n");
    printf("  -t  Set program to text mode.\n");
    printf("  -h  Show help.\n");
    printf("  -v  Show version.\n");
    return PROGRAM_SUCCESS;
}

static int run_version()
{
    printf("%s\n", PROGRAM_VERSION);
    return PROGRAM_SUCCESS;
}
