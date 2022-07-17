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
static void crash(void);

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
    int direction, distanceToOverlow, willOverflow;

    direction = right >= 0 ? 1 : -1;
    distanceToOverlow = direction == 1 ? INT_MAX - right : INT_MIN - right;
    willOverflow = direction == 1 ? left > distanceToOverlow : left < distanceToOverlow;

    if (willOverflow)
    {
        return direction == 1 ? INT_MAX + left - distanceToOverlow : -(INT_MAX - (left - distanceToOverlow - 1));
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

static void crash(void)
{
    exit(PROGRAM_FAILURE);
}
