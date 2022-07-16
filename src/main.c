#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define PROGRAM_VERSION "v0.0.0"
#define PROGRAM_NAME "langnd"
#define PROGRAM_SUCCESS 0
#define PROGRAM_FAILURE 1

static int run_help();
static int run_version();
static void crash_with_message(char *format, ...);
static void crash(void);

int main(int argumentsCount, char **arguments)
{
    int argumentsIndex;

    for (argumentsIndex = 1; argumentsIndex < argumentsCount; argumentsIndex++) 
    {
        char *argument;
        size_t argumentLength;
        int argumentIndex;

        argument = arguments[argumentsIndex];
        argumentLength = strlen(argument);

        if (argumentLength == 0 || strcmp(argument, "--") == 0)
        {
            break;
        }

        for (argumentIndex = 1; argumentIndex < argumentLength; argumentIndex++)
        {
            char argumentShorthand;

            argumentShorthand = argument[argumentIndex];

            switch (argumentShorthand)
            {
                case 'h':
                    return run_help();

                case 'v':
                    return run_version();
                
                default:
                    crash_with_message("illegal option %c", argumentShorthand);
                    break;
            }
        }
    }

    return PROGRAM_SUCCESS;
}

static int run_help()
{
    printf("Usage:\n");
    printf("  %s\n", PROGRAM_NAME);
    printf("\n");
    printf("Options:\n");
    printf("  -h  Show help.\n");
    printf("  -v  Show version.\n");
    return PROGRAM_SUCCESS;
}

static int run_version()
{
    printf("%s\n", PROGRAM_VERSION);
    return PROGRAM_SUCCESS;
}

static void crash_with_message(char *format, ...)
{
    va_list arguments;

    va_start(arguments, format);
    fprintf(stderr, "%s: ", PROGRAM_NAME);
    vfprintf(stderr, format, arguments);
    fprintf(stderr, "\n");
    va_end(arguments);

    crash();
}

static void crash(void)
{
    exit(PROGRAM_FAILURE);
}
