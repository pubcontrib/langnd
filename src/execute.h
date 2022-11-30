#ifndef EXECUTE_H
#define EXECUTE_H

#include "utility.h"

typedef struct
{
    string_t *errorMessage;
    string_t *hintMessage;
} outcome_t;

outcome_t *execute(string_t *code);
void destroy_outcome(outcome_t *outcome);

#endif
