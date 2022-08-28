#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include "utility.h"

static void destroy_chain(map_chain_t *chain, void (*destroy)(void *));
static map_chain_t *create_map_chain(char *key, void *value, map_chain_t *next);
static map_t *create_map(int (*hash)(char *), void (*destroy)(void *), size_t length, size_t capacity, map_chain_t **chains);
static void resize_map(map_t *map);
static int integer_digits(int integer);
static int integer_power(int a, int b);
static void crash(void);

void assure_portable_environment()
{
    if (INT_MIN > -PORTABLE_INT_LIMIT || INT_MAX < PORTABLE_INT_LIMIT)
    {
        crash_with_message("missing portable requirement INT_LIMIT");
    }

    if (LONG_MIN > -PORTABLE_LONG_LIMIT || LONG_MAX < PORTABLE_LONG_LIMIT)
    {
        crash_with_message("missing portable requirement LONG_LIMIT");
    }

    if (((size_t) -1) < PORTABLE_INT_LIMIT)
    {
        crash_with_message("missing portable requirement POINTER_LIMIT");
    }
}

map_t *empty_map(int (*hash)(char *), void (*destroy)(void *), size_t capacity)
{
    map_chain_t **chains;

    chains = allocate_with_zeros(capacity, sizeof(map_t *));

    return create_map(hash, destroy, 0, capacity, chains);
}

int has_map_item(map_t *map, char *key)
{
    return get_map_item(map, key) != NULL;
}

void *get_map_item(map_t *map, char *key)
{
    map_chain_t *chain;
    int hash, index;

    hash = abs(map->hash(key));
    index = div(hash, map->capacity).rem;

    for (chain = map->chains[index]; chain != NULL; chain = chain->next)
    {
        if (strcmp(key, chain->key) == 0)
        {
            return chain->value;
        }
    }

    return NULL;
}

void set_map_item(map_t *map, char *key, void *value)
{
    map_chain_t *chain, *last, *created;
    int hash, index;

    hash = abs(map->hash(key));
    index = div(hash, map->capacity).rem;

    for (chain = map->chains[index], last = NULL; chain != NULL; chain = chain->next)
    {
        if (strcmp(key, chain->key) == 0)
        {
            free(key);
            map->destroy(chain->value);
            chain->value = value;
            return;
        }

        last = chain;
    }

    created = create_map_chain(key, value, NULL);

    if (last)
    {
        last->next = created;
    }
    else
    {
        map->chains[index] = created;
    }

    map->length += 1;

    if (map->length == map->capacity)
    {
        resize_map(map);
    }
}

void remove_map_item(map_t *map, char *key)
{
    map_chain_t *chain, *previous;
    int hash, index;

    hash = abs(map->hash(key));
    index = div(hash, map->capacity).rem;

    for (chain = map->chains[index], previous = NULL; chain != NULL; chain = chain->next)
    {
        if (strcmp(key, chain->key) == 0)
        {
            if (previous)
            {
                previous->next = chain->next;
            }
            else
            {
                map->chains[index] = chain->next;
            }

            chain->next = NULL;
            destroy_chain(chain, map->destroy);
            map->length -= 1;

            return;
        }

        previous = chain;
    }
}

void destroy_map(map_t *map)
{
    if (map->chains)
    {
        size_t index;

        for (index = 0; index < map->capacity; index++)
        {
            map_chain_t *chain;

            chain = map->chains[index];

            if (chain)
            {
                destroy_chain(chain, map->destroy);
            }
        }

        free(map->chains);
    }

    free(map);
}

int add_numbers(number_t left, number_t right, number_t *out)
{
    long sum;

    sum = left + right;

    if (sum < INT_MIN || sum > INT_MAX)
    {
        return 1;
    }

    (*out) = (int) sum;

    return 0;
}

int subtract_numbers(number_t left, number_t right, number_t *out)
{
    long difference;

    difference = left - right;

    if (difference < INT_MIN || difference > INT_MAX)
    {
        return 1;
    }

    (*out) = (int) difference;

    return 0;
}

int string_to_number(char *text, number_t *out)
{
    int number, whole, fraction, wholeIndex, fractionIndex, negative, decimal, point;
    size_t index, length;

    whole = 0;
    fraction = 0;
    wholeIndex = 0;
    fractionIndex = 0;
    negative = 0;
    decimal = 0;
    point = -1;
    length = strlen(text);

    for (index = 0; index < length; index++)
    {
        if (text[index] == '.')
        {
            point = index;
            break;
        }
    }

    for (index = 0; index < length; index++)
    {
        char symbol;

        symbol = text[index];

        if (symbol >= '0' && symbol <= '9')
        {
            int digit, places;

            digit = symbol - '0';

            if (!decimal)
            {
                places = (point == -1 ? length : point) - (negative ? 1 : 0) - wholeIndex++;
                whole += digit * integer_power(10, places - 1);

                if (whole > 32767)
                {
                    return 1;
                }
            }
            else
            {
                places = length - point - fractionIndex++ - 1;
                fraction += digit * integer_power(10, places - 1);

                if (fractionIndex > 6)
                {
                    return 1;
                }
            }
        }
        else if (symbol == '-')
        {
            if (index == 0)
            {
                negative = 1;
                continue;
            }
            else
            {
                return 1;
            }
        }
        else if (symbol == '.')
        {
            if (index > 0)
            {
                decimal = 1;
                continue;
            }
            else
            {
                return 1;
            }
        }
        else
        {
            return 1;
        }
    }

    number = (whole * 65536) + (fraction * 65536L) / integer_power(10, fractionIndex);

    if (negative)
    {
        number *= -1;
    }

    (*out) = number;

    return 0;
}

int integer_to_number(int integer, number_t *out)
{
    if (integer < -32767 || integer > 32767)
    {
        return 1;
    }

    (*out) = integer * 65536;

    return 0;
}

char *represent_number(number_t number)
{
    char *string;
    int whole, fraction, negative, decimal, decimalWidth;
    size_t wholeDigits, fractionDigits, length, index, zero;

    whole = number / 65536;
    wholeDigits = integer_digits(whole);
    negative = number < 0;

    if (!negative || (number & 65535) == 0)
    {
        fraction = ((number & 65535) * 1000000l) / 65536;
    }
    else
    {
        fraction = ((65536 - (number & 65535)) * 1000000l) / 65536;
    }

    fractionDigits = integer_digits(fraction);
    decimal = fraction > 0;
    decimalWidth = 6;
    length = (negative ? 1 : 0) + wholeDigits + (decimal ? (decimalWidth + 1) : 0);
    string = allocate(sizeof(char) * length + 1);
    index = (negative ? 1 : 0) + wholeDigits - 1;

    if (negative)
    {
        string[0] = '-';
        whole *= -1;
    }

    if (whole == 0)
    {
        string[index] = '0';
    }

    while (whole > 0)
    {
        int next, digit;

        next = whole / 10;
        digit = whole - (next * 10);
        string[index--] = '0' + digit;
        whole = next;
    }

    if (decimal)
    {
        string[(negative ? 1 : 0) + wholeDigits] = '.';
    }

    index = length - 1;

    while (fraction > 0)
    {
        int next, digit;

        next = fraction / 10;
        digit = fraction - (next * 10);
        string[index--] = '0' + digit;
        fraction = next;
    }

    if (decimal)
    {
        for (zero = fractionDigits; zero < decimalWidth; zero++)
        {
            string[index--] = '0';
        }
    }

    string[length] = '\0';

    return string;
}

int hash_string(char *string)
{
    int hash;
    size_t index, length;

    hash = 0;
    length = strlen(string);

    for (index = 0; index < length; index++)
    {
        hash = add_with_overflow(hash, string[index]);
    }

    return hash;
}

int add_with_overflow(int left, int right)
{
    int min, max, direction, distanceToOverflow, willOverflow;

    min = -PORTABLE_INT_LIMIT;
    max = PORTABLE_INT_LIMIT;
    direction = right >= 0 ? 1 : -1;
    distanceToOverflow = direction == 1 ? max - right : min - right;
    willOverflow = direction == 1 ? left > distanceToOverflow : left < distanceToOverflow;

    if (willOverflow)
    {
        return direction == 1 ? min + (left - distanceToOverflow - 1) : max - (distanceToOverflow - left) + 1;
    }
    else
    {
        return left + right;
    }
}

char *copy_string(char *string)
{
    char *copy;
    size_t length;

    length = strlen(string);
    copy = allocate(length + 1);
    memcpy(copy, string, length);
    copy[length] = '\0';

    return copy;
}

void *allocate(size_t size)
{
    void *memory;

    if (size == 0)
    {
        crash_with_message("zero memory requested");
    }

    memory = malloc(size);

    if (!memory)
    {
        crash_with_message("memory allocation failed");
    }

    return memory;
}

void *allocate_with_zeros(size_t number, size_t size)
{
    void *memory;

    if (number == 0 || size == 0)
    {
        crash_with_message("zero memory requested");
    }

    memory = calloc(number, size);

    if (!memory)
    {
        crash_with_message("memory allocation failed");
    }

    return memory;
}

void crash_with_message(char *format, ...)
{
    va_list arguments;

    va_start(arguments, format);
    fprintf(stderr, "%s: ", PROGRAM_NAME);
    vfprintf(stderr, format, arguments);
    fprintf(stderr, "\n");
    va_end(arguments);

    crash();
}

static void destroy_chain(map_chain_t *chain, void (*destroy)(void *))
{
    if (chain->key)
    {
        free(chain->key);
    }

    if (chain->value)
    {
        destroy(chain->value);
    }

    if (chain->next)
    {
        destroy_chain(chain->next, destroy);
    }

    free(chain);
}

static map_chain_t *create_map_chain(char *key, void *value, map_chain_t *next)
{
    map_chain_t *chain;

    chain = allocate(sizeof(map_chain_t));
    chain->key = key;
    chain->value = value;
    chain->next = next;

    return chain;
}

static map_t *create_map(int (*hash)(char *), void (*destroy)(void *), size_t length, size_t capacity, map_chain_t **chains)
{
    map_t *map;

    map = allocate(sizeof(map_t));
    map->hash = hash;
    map->destroy = destroy;
    map->length = length;
    map->capacity = capacity;
    map->chains = chains;

    return map;
}

static void resize_map(map_t *map)
{
    map_chain_t **existing, **chains;
    map_chain_t *chain;
    size_t expand, fill, index;

    existing = map->chains;
    expand = map->capacity * 2;
    fill = map->capacity;

    chains = allocate_with_zeros(expand, sizeof(map_chain_t *));

    map->capacity = expand;
    map->length = 0;
    map->chains = chains;

    for (index = 0; index < fill; index++)
    {
        for (chain = existing[index]; chain != NULL; chain = chain->next)
        {
            set_map_item(map, chain->key, chain->value);

            chain->key = NULL;
            chain->value = NULL;
        }
    }

    for (index = 0; index < fill; index++)
    {
        chain = existing[index];

        if (chain)
        {
            destroy_chain(chain, map->destroy);
        }
    }

    free(existing);
}

static int integer_digits(int integer)
{
    int digits;

    if (integer < 0)
    {
        integer *= -1;
    }

    for (digits = 0; integer > 0 || digits == 0; digits++)
    {
        integer /= 10;
    }

    return digits;
}

static int integer_power(int base, int power)
{
    int product;

    if (power == 0)
    {
        return 1;
    }

    product = 1;

    for (; power > 0; power--)
    {
        product *= base;
    }

    return product;
}

static void crash(void)
{
    exit(PROGRAM_FAILURE);
}
