#ifndef EXECUTE_H
#define EXECUTE_H

#include "utility.h"

typedef enum
{
    VALUE_EFFECT_NONE,
    VALUE_EFFECT_RETURN,
    VALUE_EFFECT_BREAK,
    VALUE_EFFECT_CONTINUE,
    VALUE_EFFECT_THROW
} value_effect_t;

typedef struct
{
    map_t *elements;
    value_effect_t effect;
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
