#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "execute.h"
#include "utility.h"

static int run_text(char *text);
static int run_file(char *file);
static int run_help();
static int run_version();

typedef enum
{
    PROGRAM_MODE_TEXT,
    PROGRAM_MODE_FILE,
    PROGRAM_MODE_HELP,
    PROGRAM_MODE_VERSION
} program_mode_t;

int main(int argumentsCount, char **arguments)
{
    program_mode_t mode;
    int argumentsIndex;

    mode = PROGRAM_MODE_FILE;

    for (argumentsIndex = 1; argumentsIndex < argumentsCount; argumentsIndex++)
    {
        char *argument;
        size_t argumentIndex, argumentLength;

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

                case 'f':
                    mode = PROGRAM_MODE_FILE;
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

        case PROGRAM_MODE_FILE:
        {
            if (argumentsIndex < argumentsCount)
            {
                return run_file(arguments[argumentsIndex++]);
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
            crash_with_message("unsupported branch invoked");
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

static int run_file(char *file)
{
    FILE *handle;
    int status;
    char *text;
    size_t length;

    handle = fopen(file, "rb");

    if (!handle)
    {
        crash_with_message("missing script file %s", file);
    }

    fseek(handle, 0, SEEK_END);
    length = ftell(handle);
    fseek(handle, 0, SEEK_SET);

    text = allocate(sizeof(char) * (length + 1));
    fread(text, 1, length, handle);
    text[length] = '\0';

    if (ferror(handle))
    {
        free(text);
        fclose(handle);

        crash_with_message("script file read failed %s", file);
    }

    fclose(handle);
    status = run_text(text);
    free(text);

    return status;
}

static int run_help()
{
    printf("Usage:\n");
    printf("  %s script\n", PROGRAM_NAME);
    printf("  %s -t script\n", PROGRAM_NAME);
    printf("  %s -f script\n", PROGRAM_NAME);
    printf("\n");
    printf("Options:\n");
    printf("  -t  Set program to text mode.\n");
    printf("  -f  Set program to file mode. Default mode.\n");
    printf("  -h  Show help.\n");
    printf("  -v  Show version.\n");
    return PROGRAM_SUCCESS;
}

static int run_version()
{
    printf("%s\n", PROGRAM_VERSION);
    return PROGRAM_SUCCESS;
}
