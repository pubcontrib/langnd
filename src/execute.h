#ifndef EXECUTE_H
#define EXECUTE_H

#include <stddef.h>

typedef struct
{
    char *errorMessage;
    char *hintMessage;
} outcome_t;

outcome_t *execute(char *code);
void destroy_outcome(outcome_t *outcome);

#endif
