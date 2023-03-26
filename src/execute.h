#ifndef EXECUTE_H
#define EXECUTE_H

#include "utility.h"

typedef struct
{
    map_t *elements;
} machine_t;

typedef struct
{
    string_t *errorMessage;
    string_t *hintMessage;
} outcome_t;

machine_t *empty_machine();
outcome_t *execute(string_t *code, machine_t *machine);
void destroy_machine(machine_t *machine);
void destroy_outcome(outcome_t *outcome);

#endif
