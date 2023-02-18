#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include "utility.h"

static void resize_map(map_t *map);
static map_t *create_map(int (*hash)(const string_t *), void (*destroy)(void *), size_t length, size_t capacity, map_chain_t **chains);
static map_chain_t *create_map_chain(string_t *key, void *value, map_chain_t *next);
static void destroy_map_chain(map_chain_t *chain, void (*destroy)(void *));
static list_t *create_list(void (*destroy)(void *), size_t capacity, size_t length, void **items);
static int compare_strings_unsafe(const void *left, const void *right);
static unsigned int integer_digits(int integer);
static int integer_power(int a, int b);
static void crash(void);

void ensure_portable_environment()
{
    if (INT_MIN > -PORTABLE_INT_LIMIT || INT_MAX < PORTABLE_INT_LIMIT)
    {
        crash_with_message("missing portable requirement INT_LIMIT");
    }

    if (LONG_MIN > -PORTABLE_LONG_LIMIT || LONG_MAX < PORTABLE_LONG_LIMIT)
    {
        crash_with_message("missing portable requirement LONG_LIMIT");
    }

    if (SIZE_LIMIT < PORTABLE_INT_LIMIT)
    {
        crash_with_message("missing portable requirement POINTER_LIMIT");
    }
}

int compare_values(const value_t *left, const value_t *right)
{
    if (left->type < right->type)
    {
        return -1;
    }
    else if (left->type > right->type)
    {
        return 1;
    }

    switch (left->type)
    {
        case VALUE_TYPE_NULL:
            return 0;

        case VALUE_TYPE_BOOLEAN:
        {
            boolean_t x, y;

            x = view_boolean(left);
            y = view_boolean(right);

            return x == y ? 0 : (x < y ? -1 : 1);
        }

        case VALUE_TYPE_NUMBER:
        {
            number_t x, y;

            x = view_number(left);
            y = view_number(right);

            return x == y ? 0 : (x < y ? -1 : 1);
        }

        case VALUE_TYPE_STRING:
            return compare_strings(view_string(left), view_string(right));

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
            string_t **leftKeys, **rightKeys;
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
                string_t *leftKey, *rightKey;
                value_t *leftValue, *rightValue;
                int different;

                if (index == rightMap->length)
                {
                    direction = 1;
                    break;
                }

                leftKey = leftKeys[index];
                rightKey = rightKeys[index];
                different = compare_strings(leftKey, rightKey);

                if (different)
                {
                    direction = different < 0 ? -1 : 1;
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

        case VALUE_TYPE_FUNCTION:
        {
            function_t *leftFunction, *rightFunction;

            leftFunction = view_function(left);
            rightFunction = view_function(right);

            return compare_strings(leftFunction->source, rightFunction->source);
        }

        default:
            crash_with_message("unsupported branch invoked");
            return 0;
    }
}

string_t *represent_value(const value_t *value)
{
    switch (value->type)
    {
        case VALUE_TYPE_NULL:
            return cstring_to_string("null");

        case VALUE_TYPE_BOOLEAN:
            return view_boolean(value) == BOOLEAN_TRUE ? cstring_to_string("true") : cstring_to_string("false");

        case VALUE_TYPE_NUMBER:
            return represent_number(view_number(value));

        case VALUE_TYPE_STRING:
        {
            string_t *source;
            size_t escapeCount, index;

            source = view_string(value);
            escapeCount = 0;

            for (index = 0; index < source->length; index++)
            {
                unsigned char symbol;

                symbol = source->bytes[index];

                if (symbol == '\t' || symbol == '\n' || symbol == '\r' || symbol == '"'|| symbol == '\\')
                {
                    escapeCount++;
                }
                else if (symbol < 32 || symbol > 126)
                {
                    escapeCount += 4;
                }
            }

            if (escapeCount == 0)
            {
                char *destination;

                destination = allocate(source->length + 2, sizeof(char));
                destination[0] = '"';
                memcpy(destination + 1, source->bytes, source->length);
                destination[source->length + 1] = '"';

                return create_string(destination, source->length + 2);
            }
            else
            {
                char *destination;
                size_t placement;

                destination = allocate(source->length + escapeCount + 2, sizeof(char));
                placement = 0;
                destination[placement++] = '"';

                for (index = 0; index < source->length; index++)
                {
                    unsigned char symbol;

                    symbol = source->bytes[index];

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
                        {
                            if (symbol >= 32 && symbol <= 126)
                            {
                                destination[placement++] = symbol;
                                break;
                            }
                            else
                            {
                                number_t number;
                                string_t *represent;

                                if (integer_to_number(symbol, &number) != 0)
                                {
                                    crash_with_message("unsupported branch invoked");
                                }

                                represent = represent_number(number);

                                destination[placement++] = '\\';
                                destination[placement++] = 'a';

                                if (represent->length == 1)
                                {
                                    destination[placement++] = '0';
                                    destination[placement++] = '0';
                                    destination[placement++] = represent->bytes[0];
                                }
                                else if (represent->length == 2)
                                {
                                    destination[placement++] = '0';
                                    destination[placement++] = represent->bytes[0];
                                    destination[placement++] = represent->bytes[1];
                                }
                                else if (represent->length == 3)
                                {
                                    destination[placement++] = represent->bytes[0];
                                    destination[placement++] = represent->bytes[1];
                                    destination[placement++] = represent->bytes[2];
                                }
                                else
                                {
                                    crash_with_message("unsupported branch invoked");
                                }

                                destroy_string(represent);
                            }
                        }
                    }
                }

                destination[placement++] = '"';

                return create_string(destination, source->length + escapeCount + 2);
            }
        }

        case VALUE_TYPE_LIST:
        {
            string_t *buffer;
            list_t *list;
            size_t index;
            int first;

            list = view_list(value);
            buffer = cstring_to_string("[");
            first = 1;

            for (index = 0; index < list->length; index++)
            {
                string_t *item;

                if (!first)
                {
                    extend_string_by_cstring(buffer, ", ");
                }
                else
                {
                    first = 0;
                }

                item = represent_value(list->items[index]);
                extend_string_by_string(buffer, item);
                destroy_string(item);
            }

            extend_string_by_cstring(buffer, "]");

            return buffer;
        }

        case VALUE_TYPE_MAP:
        {
            string_t *buffer;
            map_t *map;
            string_t **keys;
            size_t index;
            int first;

            map = view_map(value);
            keys = list_map_keys(map);
            buffer = cstring_to_string("{");
            first = 1;

            for (index = 0; index < map->length; index++)
            {
                string_t *key, *representation;
                value_t *value, *keyString;

                if (!first)
                {
                    extend_string_by_cstring(buffer, ", ");
                }
                else
                {
                    first = 0;
                }

                key = keys[index];
                keyString = steal_string(key);
                representation = represent_value(keyString);
                extend_string_by_string(buffer, representation);
                destroy_string(representation);
                keyString->data = NULL;
                destroy_value(keyString);

                extend_string_by_cstring(buffer, ": ");

                value = get_map_item(map, key);
                representation = represent_value(value);
                extend_string_by_string(buffer, representation);
                destroy_string(representation);
            }

            if (keys)
            {
                free(keys);
            }

            extend_string_by_cstring(buffer, "}");

            return buffer;
        }

        case VALUE_TYPE_FUNCTION:
        {
            function_t *function;

            function = value->data;

            return copy_string(function->source);
        }

        default:
            crash_with_message("unsupported branch invoked");
            return NULL;
    }
}

boolean_t view_boolean(const value_t *value)
{
    if (value->type == VALUE_TYPE_BOOLEAN)
    {
        return ((boolean_t *) value->data)[0];
    }
    else
    {
        crash_with_message("unsupported branch invoked");
        return BOOLEAN_FALSE;
    }
}

number_t view_number(const value_t *value)
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

string_t *view_string(const value_t *value)
{
    if (value->type == VALUE_TYPE_STRING)
    {
        return (string_t *) value->data;
    }
    else
    {
        crash_with_message("unsupported branch invoked");
        return NULL;
    }
}

list_t *view_list(const value_t *value)
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

map_t *view_map(const value_t *value)
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

function_t *view_function(const value_t *value)
{
    if (value->type == VALUE_TYPE_FUNCTION)
    {
        return (function_t *) value->data;
    }
    else
    {
        crash_with_message("unsupported branch invoked");
        return NULL;
    }
}

value_t *new_null()
{
    value_t *value;

    value = allocate(1, sizeof(value_t));
    value->type = VALUE_TYPE_NULL;
    value->data = NULL;
    value->owners = 1;

    return value;
}

value_t *new_boolean(boolean_t boolean)
{
    value_t *value;
    boolean_t *data;

    data = allocate(1, sizeof(boolean_t));
    data[0] = boolean;
    value = allocate(1, sizeof(value_t));
    value->type = VALUE_TYPE_BOOLEAN;
    value->data = data;
    value->owners = 1;

    return value;
}

value_t *new_number(number_t number)
{
    value_t *value;
    number_t *data;

    data = allocate(1, sizeof(number_t));
    data[0] = number;
    value = allocate(1, sizeof(value_t));
    value->type = VALUE_TYPE_NUMBER;
    value->data = data;
    value->owners = 1;

    return value;
}

value_t *steal_string(string_t *string)
{
    value_t *value;

    value = allocate(1, sizeof(value_t));
    value->type = VALUE_TYPE_STRING;
    value->data = string;
    value->owners = 1;

    return value;
}

value_t *steal_list(list_t *list)
{
    value_t *value;

    value = allocate(1, sizeof(value_t));
    value->type = VALUE_TYPE_LIST;
    value->data = list;
    value->owners = 1;

    return value;
}

value_t *steal_map(map_t *map)
{
    value_t *value;

    value = allocate(1, sizeof(value_t));
    value->type = VALUE_TYPE_MAP;
    value->data = map;
    value->owners = 1;

    return value;
}

value_t *steal_function(function_t *function)
{
    value_t *value;

    value = allocate(1, sizeof(value_t));
    value->type = VALUE_TYPE_FUNCTION;
    value->data = function;
    value->owners = 1;

    return value;
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
                free(value->data);
                break;

            case VALUE_TYPE_STRING:
                destroy_string(value->data);
                break;

            case VALUE_TYPE_LIST:
                destroy_list(value->data);
                break;

            case VALUE_TYPE_MAP:
                destroy_map(value->data);
                break;

            case VALUE_TYPE_FUNCTION:
                destroy_function(value->data);
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

    if (value->owners == 0)
    {
        destroy_value(value);
    }
}

function_t *create_function(list_t *expressions, string_t *source)
{
    function_t *function;

    function = malloc(sizeof(function_t));
    function->expressions = expressions;
    function->source = source;

    return function;
}

void destroy_function(function_t *function)
{
    if (function->expressions)
    {
        destroy_list(function->expressions);
    }

    if (function->source)
    {
        destroy_string(function->source);
    }

    free(function);
}

string_t **list_map_keys(const map_t *map)
{
    string_t **keys;
    size_t index, placement;

    if (map->length == 0)
    {
        return NULL;
    }

    keys = allocate(map->length, sizeof(string_t *));

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

    qsort(keys, map->length, sizeof(string_t *), compare_strings_unsafe);

    return keys;
}

int has_map_item(const map_t *map, const string_t *key)
{
    return get_map_item(map, key) != NULL;
}

void *get_map_item(const map_t *map, const string_t *key)
{
    map_chain_t *chain;
    int hash, index;

    hash = abs(map->hash(key));
    index = div(hash, map->capacity).rem;

    for (chain = map->chains[index]; chain != NULL; chain = chain->next)
    {
        if (compare_strings(key, chain->key) == 0)
        {
            return chain->value;
        }
    }

    return NULL;
}

void set_map_item(map_t *map, string_t *key, void *value)
{
    map_chain_t *chain, *last, *created;
    int hash, index;

    hash = abs(map->hash(key));
    index = div(hash, map->capacity).rem;

    for (chain = map->chains[index], last = NULL; chain != NULL; chain = chain->next)
    {
        if (compare_strings(key, chain->key) == 0)
        {
            destroy_string(key);
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

void unset_map_item(map_t *map, const string_t *key)
{
    map_chain_t *chain, *previous;
    int hash, index;

    hash = abs(map->hash(key));
    index = div(hash, map->capacity).rem;

    for (chain = map->chains[index], previous = NULL; chain != NULL; chain = chain->next)
    {
        if (compare_strings(key, chain->key) == 0)
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
            destroy_map_chain(chain, map->destroy);
            map->length -= 1;

            return;
        }

        previous = chain;
    }
}

map_t *empty_map(int (*hash)(const string_t *), void (*destroy)(void *), size_t capacity)
{
    map_chain_t **chains;

    chains = allocate_with_zeros(capacity, sizeof(map_t *));

    return create_map(hash, destroy, 0, capacity, chains);
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
                destroy_map_chain(chain, map->destroy);
            }
        }

        free(map->chains);
    }

    free(map);
}

void add_list_item(list_t *list, void *value)
{
    if (list->length == list->capacity)
    {
        list->capacity *= 2;
        list->items = reallocate(list->items, list->capacity, sizeof(void *));
    }

    list->items[list->length++] = value;
}

list_t *empty_list(void (*destroy)(void *), size_t capacity)
{
    return create_list(destroy, capacity, 0, allocate(capacity, sizeof(void *)));
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

string_t *cstring_to_string(const char *cstring)
{
    char *bytes;
    size_t length;

    length = strlen(cstring);

    if (length > 0)
    {
        bytes = allocate(length, sizeof(char));
        memcpy(bytes, cstring, length);
    }
    else
    {
        bytes = NULL;
    }

    return create_string(bytes, length);
}

char *string_to_cstring(const string_t *string)
{
    char *cstring;

    cstring = allocate(string->length + 1, sizeof(char));

    if (string->length > 0)
    {
        memcpy(cstring, string->bytes, string->length);
    }

    cstring[string->length] = '\0';

    return cstring;
}

int hash_string(const string_t *string)
{
    int hash;
    size_t index;

    hash = 0;

    for (index = 0; index < string->length; index++)
    {
        hash = add_with_overflow(hash, string->bytes[index]);
    }

    return hash;
}

string_t *copy_string(const string_t *string)
{
    char *bytes;
    size_t length;

    length = string->length;

    if (length > 0)
    {
        bytes = allocate(length, sizeof(char));
        memcpy(bytes, string->bytes, length);
    }
    else
    {
        bytes = NULL;
    }

    return create_string(bytes, length);
}

string_t *merge_strings(const string_t *left, const string_t *right)
{
    char *sum;
    size_t length;

    length = left->length + right->length;

    if (length > 0)
    {
        sum = allocate(length, sizeof(char));
        memcpy(sum, left->bytes, left->length);
        memcpy(sum + left->length, right->bytes, right->length);
    }
    else
    {
        sum = NULL;
    }

    return create_string(sum, length);
}

void extend_string_by_cstring(string_t *origin, const char *extension)
{
    char *bytes;
    size_t length;

    length = origin->length + strlen(extension);

    if (length > 0)
    {
        bytes = reallocate(origin->bytes, length, sizeof(char));
        memcpy(bytes + origin->length, extension, strlen(extension));
    }
    else
    {
        bytes = NULL;
    }

    origin->bytes = bytes;
    origin->length = length;
}

void extend_string_by_string(string_t *origin, const string_t *extension)
{
    char *bytes;
    size_t length;

    length = origin->length + extension->length;

    if (length > 0)
    {
        bytes = reallocate(origin->bytes, length, sizeof(char));
        memcpy(bytes + origin->length, extension->bytes, extension->length);
    }
    else
    {
        bytes = NULL;
    }

    origin->bytes = bytes;
    origin->length = length;
}

int compare_strings(const string_t *left, const string_t *right)
{
    size_t index;

    for (index = 0; index < left->length; index++)
    {
        unsigned char x, y;

        if (index == right->length)
        {
            return 1;
        }

        x = left->bytes[index];
        y = right->bytes[index];

        if (x != y)
        {
            return x < y ? -1 : 1;
        }
    }

    if (index < right->length)
    {
        return -1;
    }

    return 0;
}

int is_keyword_match(const string_t *left, const char *right)
{
    size_t index;

    index = 0;

    while (right[index])
    {
        if (index >= left->length)
        {
            return 0;
        }

        if (left->bytes[index] != right[index])
        {
            return 0;
        }

        index++;
    }

    return index == left->length;
}

string_t *empty_string()
{
    return create_string(NULL, 0);
}

string_t *create_string(char *bytes, size_t length)
{
    string_t *string;

    string = allocate(1, sizeof(string_t));
    string->bytes = bytes;
    string->length = length;

    return string;
}

void destroy_string(string_t *string)
{
    if (string->bytes)
    {
        free(string->bytes);
    }

    free(string);
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

int string_to_number(const string_t *text, number_t *out)
{
    int number, whole, fraction, wholeIndex, fractionIndex, negative, decimal, point;
    size_t index;

    whole = 0;
    fraction = 0;
    wholeIndex = 0;
    fractionIndex = 0;
    negative = 0;
    decimal = 0;
    point = -1;

    for (index = 0; index < text->length; index++)
    {
        if (text->bytes[index] == '.')
        {
            point = index;
            break;
        }
    }

    if (point > 6 || (point == -1 && text->length > 6))
    {
        return 1;
    }

    for (index = 0; index < text->length; index++)
    {
        char symbol;

        symbol = text->bytes[index];

        if (symbol >= '0' && symbol <= '9')
        {
            int digit, places;

            digit = symbol - '0';

            if (!decimal)
            {
                places = (point == -1 ? (int) text->length : point) - (negative ? 1 : 0) - wholeIndex++;
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
        else if (symbol == '+')
        {
            if (index == 0)
            {
                negative = 0;
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

string_t *represent_number(number_t number)
{
    char *bytes;
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
    bytes = allocate(length, sizeof(char));
    index = (negative ? 1 : 0) + wholeDigits - 1;

    if (negative)
    {
        bytes[0] = '-';
        whole *= -1;
    }

    if (whole == 0)
    {
        bytes[index] = '0';
    }

    while (whole > 0)
    {
        int next, digit;

        next = whole / 10;
        digit = whole - (next * 10);
        bytes[index--] = '0' + digit;
        whole = next;
    }

    if (decimal)
    {
        bytes[(negative ? 1 : 0) + wholeDigits] = '.';
    }

    index = length - 1;

    while (fraction > 0)
    {
        int next, digit;

        next = fraction / 10;
        digit = fraction - (next * 10);
        bytes[index--] = '0' + digit;
        fraction = next;
    }

    if (decimal)
    {
        for (zero = fractionDigits; zero < decimalWidth; zero++)
        {
            bytes[index--] = '0';
        }
    }

    return create_string(bytes, length);
}

number_t truncate_number(number_t number)
{
    return (number / 65536) * 65536;
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

void *allocate(size_t number, size_t size)
{
    void *memory;

    if (number == 0 || size == 0)
    {
        crash_with_message("zero memory requested");
    }

    if (number > SIZE_LIMIT / size)
    {
        crash_with_message("oversized memory requested");
    }

    memory = malloc(number * size);

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

    if (number > SIZE_LIMIT / size)
    {
        crash_with_message("oversized memory requested");
    }

    memory = calloc(number, size);

    if (!memory)
    {
        crash_with_message("memory allocation failed");
    }

    return memory;
}

void *reallocate(void *memory, size_t number, size_t size)
{
    if (number == 0 || size == 0)
    {
        crash_with_message("zero memory requested");
    }

    if (number > SIZE_LIMIT / size)
    {
        crash_with_message("oversized memory requested");
    }

    memory = realloc(memory, number * size);

    if (!memory)
    {
        crash_with_message("memory allocation failed");
    }

    return memory;
}

void crash_with_message(const char *format, ...)
{
    va_list arguments;

    va_start(arguments, format);
    fprintf(stderr, "%s: ", PROGRAM_NAME);
    vfprintf(stderr, format, arguments);
    fprintf(stderr, "\n");
    va_end(arguments);

    crash();
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
            destroy_map_chain(chain, map->destroy);
        }
    }

    free(existing);
}

static map_t *create_map(int (*hash)(const string_t *), void (*destroy)(void *), size_t length, size_t capacity, map_chain_t **chains)
{
    map_t *map;

    map = allocate(1, sizeof(map_t));
    map->hash = hash;
    map->destroy = destroy;
    map->length = length;
    map->capacity = capacity;
    map->chains = chains;

    return map;
}

static map_chain_t *create_map_chain(string_t *key, void *value, map_chain_t *next)
{
    map_chain_t *chain;

    chain = allocate(1, sizeof(map_chain_t));
    chain->key = key;
    chain->value = value;
    chain->next = next;

    return chain;
}

static void destroy_map_chain(map_chain_t *chain, void (*destroy)(void *))
{
    if (chain->key)
    {
        destroy_string(chain->key);
    }

    if (chain->value)
    {
        destroy(chain->value);
    }

    if (chain->next)
    {
        destroy_map_chain(chain->next, destroy);
    }

    free(chain);
}

static list_t *create_list(void (*destroy)(void *), size_t capacity, size_t length, void **items)
{
    list_t *list;

    list = allocate(1, sizeof(list_t));
    list->destroy = destroy;
    list->capacity = capacity;
    list->length = length;
    list->items = items;

    return list;
}

static int compare_strings_unsafe(const void *left, const void *right)
{
    return compare_strings(*(string_t **) left, *(string_t **) right);
}

static unsigned int integer_digits(int integer)
{
    unsigned int digits;

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
