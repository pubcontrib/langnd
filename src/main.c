#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "execute.h"
#include "utility.h"

static int run_text(const string_t *text, int argc, char **argv, int skip);
static int run_file(const char *file, int argc, char **argv, int skip);
static int run_help();
static int run_version();
static string_t *get_detail(const map_t *details, char *key);

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
                string_t string;

                string.bytes = arguments[argumentsIndex++];
                string.length = strlen(string.bytes);

                return run_text(&string, argumentsCount, arguments, argumentsIndex);
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
                char *file;

                file = arguments[argumentsIndex++];

                return run_file(file, argumentsCount, arguments, argumentsIndex);
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

static int run_text(const string_t *text, int argc, char **argv, int skip)
{
    machine_t *machine;
    value_t *outcome;
    value_effect_t effect;

    ensure_portable_environment();

    machine = empty_machine(argc, argv, skip);
    outcome = execute(text, machine);
    effect = machine->effect;
    destroy_machine(machine);

    if (effect == VALUE_EFFECT_THROW)
    {
        string_t *message, *hint;
        map_t *details;

        details = view_map(outcome);
        message = get_detail(details, "message");

        if (outcome->type != VALUE_TYPE_MAP || !message)
        {
            crash_with_message("unsupported branch invoked");

            return PROGRAM_FAILURE;
        }

        fprintf(stderr, "%s: ", PROGRAM_NAME);
        fwrite(message->bytes, sizeof(char), message->length, stderr);
        fprintf(stderr, "\n");

        hint = get_detail(details, "hint");

        if (hint)
        {
            fprintf(stderr, "    [hint] ");
            fwrite(hint->bytes, sizeof(char), hint->length, stderr);
            fprintf(stderr, "\n");
        }

        dereference_value(outcome);

        return PROGRAM_FAILURE;
    }

    dereference_value(outcome);

    return PROGRAM_SUCCESS;
}

static int run_file(const char *file, int argc, char **argv, int skip)
{
    FILE *handle;
    int status;
    string_t text;
    char *bytes;
    long length;

    handle = fopen(file, "rb");

    if (!handle)
    {
        crash_with_message("missing script file %s", file);
    }

    fseek(handle, 0, SEEK_END);
    length = ftell(handle);
    fseek(handle, 0, SEEK_SET);

    if (length < 0L || (unsigned long) length > SIZE_LIMIT)
    {
        fclose(handle);

        crash_with_message("script file read failed %s", file);

        return PROGRAM_FAILURE;
    }
    else if (length > 0L)
    {
        size_t read;

        bytes = allocate(length, sizeof(char));
        read = fread(bytes, 1, length, handle);

        if (ferror(handle) || read != (unsigned long) length)
        {
            free(bytes);
            fclose(handle);

            crash_with_message("script file read failed %s", file);

            return PROGRAM_FAILURE;
        }
    }
    else
    {
        bytes = NULL;
    }

    fclose(handle);
    text.bytes = bytes;
    text.length = length;
    status = run_text(&text, argc, argv, skip);

    if (bytes)
    {
        free(bytes);
    }

    return status;
}

static int run_help()
{
    printf("Usage:\n");
    printf("  %s script [inputs]\n", PROGRAM_NAME);
    printf("  %s -t script [inputs]\n", PROGRAM_NAME);
    printf("  %s -f script [inputs]\n", PROGRAM_NAME);
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

static string_t *get_detail(const map_t *details, char *name)
{
    value_t *item;
    string_t key;

    key.bytes = name;
    key.length = strlen(name);

    item = get_map_item(details, &key);

    if (item && item->type == VALUE_TYPE_STRING)
    {
        return view_string(item);
    }
    else
    {
        return NULL;
    }
}
