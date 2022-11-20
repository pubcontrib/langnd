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
static list_t *create_list(void (*destroy)(void *), size_t capacity, size_t length, void **items);
static int compare_strings_unsafe(const void *left, const void *right);
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

int compare_values(value_t *left, value_t *right)
{
    if (left->type != right->type)
    {
        return left->type - right->type;
    }

    switch (left->type)
    {
        case VALUE_TYPE_NULL:
            return 0;

        case VALUE_TYPE_BOOLEAN:
            return view_boolean(left) - view_boolean(right);

        case VALUE_TYPE_NUMBER:
        {
            number_t x, y;

            x = view_number(left);
            y = view_number(right);

            return x == y ? 0 : (x < y ? -1 : 1);
        }

        case VALUE_TYPE_STRING:
            return strcmp(view_string(left), view_string(right));

        case VALUE_TYPE_LIST:
        {
            list_t *leftList, *rightList;
            size_t index;

            leftList = view_list(left);
            rightList = view_list(right);

            for (index = 0; index < leftList->length; index++)
            {
                int different;

                if (index == rightList->length)
                {
                    return 1;
                }

                different = compare_values(leftList->items[index], rightList->items[index]);

                if (different)
                {
                    return different;
                }
            }

            if (index < rightList->length)
            {
                return -1;
            }

            return 0;
        }

        case VALUE_TYPE_MAP:
        {
            map_t *leftMap, *rightMap;
            char **leftKeys, **rightKeys;
            int direction;
            size_t index;

            leftMap = view_map(left);
            rightMap = view_map(right);

            if (leftMap->length == 0 && rightMap->length == 0)
            {
                return 0;
            }

            if (leftMap->length == 0 && rightMap->length > 0)
            {
                return -1;
            }

            if (rightMap->length == 0)
            {
                return 1;
            }

            leftKeys = list_map_keys(leftMap);
            rightKeys = list_map_keys(rightMap);
            direction = 0;

            for (index = 0; index < leftMap->length; index++)
            {
                char *leftKey, *rightKey;
                value_t *leftValue, *rightValue;
                int different;

                if (index == rightMap->length)
                {
                    direction = 1;
                    break;
                }

                leftKey = leftKeys[index];
                rightKey = rightKeys[index];
                different = strcmp(leftKey, rightKey);

                if (different)
                {
                    direction = different;
                    break;
                }

                leftValue = get_map_item(leftMap, leftKey);
                rightValue = get_map_item(rightMap, rightKey);

                if (!rightValue)
                {
                    direction = -1;
                    break;
                }

                different = compare_values(leftValue, rightValue);

                if (different)
                {
                    direction = different;
                    break;
                }
            }

            free(leftKeys);
            free(rightKeys);

            if (direction == 0 && index < rightMap->length)
            {
                return -1;
            }

            return direction;
        }

        default:
            crash_with_message("unsupported branch invoked");
            return 0;
    }
}

char *represent_value(value_t *value)
{
    switch (value->type)
    {
        case VALUE_TYPE_NULL:
            return copy_string("null");

        case VALUE_TYPE_BOOLEAN:
            return view_boolean(value) == TRUE ? copy_string("true") : copy_string("false");

        case VALUE_TYPE_NUMBER:
            return represent_number(view_number(value));

        case VALUE_TYPE_STRING:
        {
            char *source, *destination;
            size_t escapeCount, length, index;

            source = view_string(value);
            escapeCount = 0;
            length = strlen(source);

            for (index = 0; index < length; index++)
            {
                char symbol;

                symbol = source[index];

                if (symbol == '\t' || symbol == '\n' || symbol == '\r' || symbol == '"'|| symbol == '\\')
                {
                    escapeCount++;
                }
            }

            if (escapeCount == 0)
            {
                destination = allocate(sizeof(char) * (length + 3));
                destination[0] = '"';
                memcpy(destination + 1, source, length);
                destination[length + 1] = '"';
                destination[length + 2] = '\0';
            }
            else
            {
                size_t placement;

                destination = allocate(sizeof(char) * (length + escapeCount + 3));
                placement = 0;
                destination[placement++] = '"';

                for (index = 0; index < length; index++)
                {
                    char symbol;

                    symbol = source[index];

                    switch (symbol)
                    {
                        case '\t':
                            destination[placement++] = '\\';
                            destination[placement++] = 't';
                            break;

                        case '\n':
                            destination[placement++] = '\\';
                            destination[placement++] = 'n';
                            break;

                        case '\r':
                            destination[placement++] = '\\';
                            destination[placement++] = 'r';
                            break;

                        case '"':
                            destination[placement++] = '\\';
                            destination[placement++] = '"';
                            break;

                        case '\\':
                            destination[placement++] = '\\';
                            destination[placement++] = '\\';
                            break;

                        default:
                            destination[placement++] = symbol;
                            break;
                    }
                }

                destination[placement++] = '"';
                destination[placement++] = '\0';
            }

            return destination;
        }

        case VALUE_TYPE_LIST:
        {
            char *buffer, *swap;
            list_t *list;
            size_t index;
            int first;

            list = view_list(value);
            buffer = copy_string("[");
            first = 1;

            for (index = 0; index < list->length; index++)
            {
                char *item;

                if (!first)
                {
                    swap = merge_strings(buffer, ", ");
                    free(buffer);
                    buffer = swap;
                }
                else
                {
                    first = 0;
                }

                item = represent_value(list->items[index]);
                swap = merge_strings(buffer, item);
                free(buffer);
                free(item);
                buffer = swap;
            }

            swap = merge_strings(buffer, "]");
            free(buffer);
            buffer = swap;

            return buffer;
        }

        case VALUE_TYPE_MAP:
        {
            char *buffer, *swap;
            map_t *map;
            char **keys;
            size_t index;
            int first;

            map = view_map(value);
            keys = list_map_keys(map);
            buffer = copy_string("{");
            first = 1;

            for (index = 0; index < map->length; index++)
            {
                char *key, *representation;
                value_t *value, *keyString;

                if (!first)
                {
                    swap = merge_strings(buffer, ", ");
                    free(buffer);
                    buffer = swap;
                }
                else
                {
                    first = 0;
                }

                key = keys[index];
                value = get_map_item(map, key);

                keyString = new_string(key);
                representation = represent_value(keyString);
                swap = merge_strings(buffer, representation);
                free(buffer);
                free(representation);
                destroy_value(keyString);
                buffer = swap;

                swap = merge_strings(buffer, ": ");
                free(buffer);
                buffer = swap;

                representation = represent_value(value);
                swap = merge_strings(buffer, representation);
                free(buffer);
                free(representation);
                buffer = swap;
            }

            if (keys)
            {
                free(keys);
            }

            swap = merge_strings(buffer, "}");
            free(buffer);
            buffer = swap;

            return buffer;
        }

        default:
            crash_with_message("unsupported branch invoked");
            return NULL;
    }
}

value_t *throw_error(char *message)
{
    value_t *value;

    value = new_string(message);
    value->thrown = 1;

    return value;
}

value_t *new_null()
{
    value_t *value;

    value = allocate(sizeof(value_t));
    value->type = VALUE_TYPE_NULL;
    value->data = NULL;
    value->thrown = 0;
    value->owners = 1;

    return value;
}

value_t *new_boolean(boolean_t boolean)
{
    value_t *value;
    boolean_t *data;

    data = allocate(sizeof(boolean_t));
    data[0] = boolean;
    value = allocate(sizeof(value_t));
    value->type = VALUE_TYPE_BOOLEAN;
    value->data = data;
    value->thrown = 0;
    value->owners = 1;

    return value;
}

value_t *new_number(number_t number)
{
    value_t *value;
    number_t *data;

    data = allocate(sizeof(number_t));
    data[0] = number;
    value = allocate(sizeof(value_t));
    value->type = VALUE_TYPE_NUMBER;
    value->data = data;
    value->thrown = 0;
    value->owners = 1;

    return value;
}

value_t *new_string(char *string)
{
    value_t *value;
    char *data;

    data = copy_string(string);
    value = allocate(sizeof(value_t));
    value->type = VALUE_TYPE_STRING;
    value->data = data;
    value->thrown = 0;
    value->owners = 1;

    return value;
}

value_t *steal_string(char *string)
{
    value_t *value;

    value = allocate(sizeof(value_t));
    value->type = VALUE_TYPE_STRING;
    value->data = string;
    value->thrown = 0;
    value->owners = 1;

    return value;
}

value_t *steal_list(list_t *list)
{
    value_t *value;

    value = allocate(sizeof(value_t));
    value->type = VALUE_TYPE_LIST;
    value->data = list;
    value->thrown = 0;
    value->owners = 1;

    return value;
}

value_t *steal_map(map_t *map)
{
    value_t *value;

    value = allocate(sizeof(value_t));
    value->type = VALUE_TYPE_MAP;
    value->data = map;
    value->thrown = 0;
    value->owners = 1;

    return value;
}

boolean_t view_boolean(value_t *value)
{
    if (value->type == VALUE_TYPE_BOOLEAN)
    {
        return ((boolean_t *) value->data)[0];
    }
    else
    {
        crash_with_message("unsupported branch invoked");
        return FALSE;
    }
}

number_t view_number(value_t *value)
{
    if (value->type == VALUE_TYPE_NUMBER)
    {
        return ((number_t *) value->data)[0];
    }
    else
    {
        crash_with_message("unsupported branch invoked");
        return 0;
    }
}

char *view_string(value_t *value)
{
    if (value->type == VALUE_TYPE_STRING)
    {
        return (char *) value->data;
    }
    else
    {
        crash_with_message("unsupported branch invoked");
        return "";
    }
}

list_t *view_list(value_t *value)
{
    if (value->type == VALUE_TYPE_LIST)
    {
        return (list_t *) value->data;
    }
    else
    {
        crash_with_message("unsupported branch invoked");
        return NULL;
    }
}

map_t *view_map(value_t *value)
{
    if (value->type == VALUE_TYPE_MAP)
    {
        return (map_t *) value->data;
    }
    else
    {
        crash_with_message("unsupported branch invoked");
        return NULL;
    }
}

void destroy_value(value_t *value)
{
    if (value->data)
    {
        switch (value->type)
        {
            case VALUE_TYPE_NULL:
            case VALUE_TYPE_BOOLEAN:
            case VALUE_TYPE_NUMBER:
            case VALUE_TYPE_STRING:
                free(value->data);
                break;

            case VALUE_TYPE_LIST:
                destroy_list(value->data);
                break;

            case VALUE_TYPE_MAP:
                destroy_map(value->data);
                break;

            default:
                crash_with_message("unsupported branch invoked");
                break;
        }
    }

    free(value);
}

void dereference_value(value_t *value)
{
    value->owners -= 1;

    if (value->owners < 1)
    {
        destroy_value(value);
    }
}

map_t *empty_map(int (*hash)(char *), void (*destroy)(void *), size_t capacity)
{
    map_chain_t **chains;

    chains = allocate_with_zeros(capacity, sizeof(map_t *));

    return create_map(hash, destroy, 0, capacity, chains);
}

char **list_map_keys(map_t *map)
{
    char **keys;
    size_t index, placement;

    if (map->length == 0)
    {
        return NULL;
    }

    keys = allocate(sizeof(char *) * map->length);

    for (index = 0, placement = 0; index < map->capacity; index++)
    {
        if (map->chains[index])
        {
            map_chain_t *chain;

            for (chain = map->chains[index]; chain != NULL; chain = chain->next)
            {
                keys[placement++] = chain->key;
            }
        }
    }

    qsort(keys, map->length, sizeof(char *), compare_strings_unsafe);

    return keys;
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

void unset_map_item(map_t *map, char *key)
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

list_t *empty_list(void (*destroy)(void *), size_t capacity)
{
    return create_list(destroy, capacity, 0, allocate(sizeof(void *) * capacity));
}

void add_list_item(list_t *list, void *value)
{
    if (list->length == list->capacity)
    {
        list->capacity *= 2;
        list->items = reallocate(list->items, sizeof(void *) * list->capacity);
    }

    list->items[list->length++] = value;
}

void destroy_list(list_t *list)
{
    size_t index;

    for (index = 0; index < list->length; index++)
    {
        list->destroy(list->items[index]);
    }

    free(list->items);
    free(list);
}

int add_numbers(number_t left, number_t right, number_t *out)
{
    long sum;

    sum = (long) left + (long) right;

    if (sum < -PORTABLE_INT_LIMIT || sum > PORTABLE_INT_LIMIT)
    {
        return 1;
    }

    (*out) = (int) sum;

    return 0;
}

int subtract_numbers(number_t left, number_t right, number_t *out)
{
    long difference;

    difference = (long) left - (long) right;

    if (difference < -PORTABLE_INT_LIMIT || difference > PORTABLE_INT_LIMIT)
    {
        return 1;
    }

    (*out) = (int) difference;

    return 0;
}

int multiply_numbers(number_t left, number_t right, number_t *out)
{
    long product;

    product = ((long) left * (long) right) / 65536L;

    if (product < -PORTABLE_INT_LIMIT || product > PORTABLE_INT_LIMIT)
    {
        return 1;
    }

    (*out) = (int) product;

    return 0;
}

int divide_numbers(number_t left, number_t right, number_t *out)
{
    long quotient;

    if (right == 0)
    {
        return 1;
    }

    quotient = ((long) left * 65536L) / (long) right;

    if (quotient < -PORTABLE_INT_LIMIT || quotient > PORTABLE_INT_LIMIT)
    {
        return 1;
    }

    (*out) = (int) quotient;

    return 0;
}

int modulo_numbers(number_t left, number_t right, number_t *out)
{
    long remainder, above, below;

    above = (long) left / 65536L;
    below = (long) right / 65536L;

    if (below == 0)
    {
        return 1;
    }

    remainder = (above % below) * 65536L;

    if (remainder < -PORTABLE_INT_LIMIT || remainder > PORTABLE_INT_LIMIT)
    {
        return 1;
    }

    (*out) = (int) remainder;

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

    if (point > 6 || (point == -1 && length > 6))
    {
        return 1;
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
                places = (point == -1 ? (int) length : point) - (negative ? 1 : 0) - wholeIndex++;
                whole += digit * integer_power(10, places - 1);

                if (whole > 32767)
                {
                    return 1;
                }
            }
            else
            {
                if (fractionIndex >= 6)
                {
                    continue;
                }

                places = 5 - fractionIndex++;
                fraction += digit * integer_power(10, places);
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

    if (fraction > 0 && fraction < 999999)
    {
        fraction += 1;
    }

    number = (whole * 65536) + ((fraction * 65536L) / 1000000);

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

int number_to_integer(number_t number, int *out)
{
    (*out) = number / 65536;

    return 0;
}

char *represent_number(number_t number)
{
    char *string;
    int whole, fraction, negative, decimal;
    size_t wholeDigits, fractionDigits, decimalWidth, length, index, zero;

    whole = number / 65536;
    wholeDigits = integer_digits(whole);
    negative = number < 0;

    if (!negative || (number & 65535) == 0)
    {
        fraction = ((number & 65535) * 1000000L) / 65536;
    }
    else
    {
        fraction = ((65536 - (number & 65535)) * 1000000L) / 65536;
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

number_t truncate_number(number_t number)
{
    return (number / 65536) * 65536;
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

char *merge_strings(char *left, char *right)
{
    char *sum;
    size_t leftLength, rightLength;

    leftLength = strlen(left);
    rightLength = strlen(right);

    sum = allocate(sizeof(char) * (leftLength + rightLength + 1));
    memcpy(sum, left, leftLength);
    memcpy(sum + leftLength, right, rightLength);
    sum[leftLength + rightLength] = '\0';

    return sum;
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

void *reallocate(void *memory, size_t size)
{
    if (size == 0)
    {
        crash_with_message("zero memory requested");
    }

    memory = realloc(memory, size);

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

static list_t *create_list(void (*destroy)(void *), size_t capacity, size_t length, void **items)
{
    list_t *list;

    list = allocate(sizeof(list_t));
    list->destroy = destroy;
    list->capacity = capacity;
    list->length = length;
    list->items = items;

    return list;
}

static int compare_strings_unsafe(const void *left, const void *right)
{
    return strcmp(*(const char **) left, *(const char **) right);
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
