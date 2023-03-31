#ifndef UTILITY_H
#define UTILITY_H

#include <stddef.h>

#define PROGRAM_VERSION "v0.36.0"
#define PROGRAM_NAME "langnd"
#define PROGRAM_SUCCESS 0
#define PROGRAM_FAILURE 1

#define PORTABLE_INT_LIMIT 2147483647
#define PORTABLE_LONG_LIMIT 9223372036854775807
#define SIZE_LIMIT (size_t) -1

typedef enum
{
    BOOLEAN_FALSE = 0,
    BOOLEAN_TRUE = 1
} boolean_t;

/**
 * 16.16 signed fixed point number.
 */
typedef int number_t;

typedef struct
{
    char *bytes;
    size_t length;
} string_t;

typedef struct
{
    void (*destroy)(void *);
    size_t capacity;
    size_t length;
    void **items;
} list_t;

typedef struct map_chain_t
{
    string_t *key;
    void *value;
    struct map_chain_t *next;
} map_chain_t;

typedef struct
{
    int (*hash)(const string_t *);
    void (*destroy)(void *);
    size_t length;
    size_t capacity;
    map_chain_t **chains;
} map_t;

typedef struct
{
    list_t *expressions;
    string_t *source;
} function_t;

typedef enum
{
    VALUE_TYPE_NULL = 0x01,
    VALUE_TYPE_BOOLEAN = 0x02,
    VALUE_TYPE_NUMBER = 0x04,
    VALUE_TYPE_STRING = 0x08,
    VALUE_TYPE_LIST = 0x10,
    VALUE_TYPE_MAP = 0x20,
    VALUE_TYPE_FUNCTION = 0x40
} value_type_t;

typedef struct
{
    value_type_t type;
    void *data;
    size_t owners;
} value_t;

void ensure_portable_environment();
int compare_values(const value_t *left, const value_t *right);
string_t *represent_value(const value_t *value);
boolean_t view_boolean(const value_t *value);
number_t view_number(const value_t *value);
string_t *view_string(const value_t *value);
list_t *view_list(const value_t *value);
map_t *view_map(const value_t *value);
function_t *view_function(const value_t *value);
value_t *new_null();
value_t *new_boolean(boolean_t boolean);
value_t *new_number(number_t number);
value_t *steal_string(string_t *string);
value_t *steal_list(list_t *list);
value_t *steal_map(map_t *map);
value_t *steal_function(function_t *function);
void destroy_value(value_t *value);
void dereference_value(value_t *value);
function_t *create_function(list_t *expressions, string_t *source);
void destroy_function(function_t *function);
string_t **list_map_keys(const map_t *map);
int has_map_item(const map_t *map, const string_t *key);
void *get_map_item(const map_t *map, const string_t *key);
void set_map_item(map_t *map, string_t *key, void *value);
void unset_map_item(map_t *map, const string_t *key);
map_t *empty_map(int (*hash)(const string_t *), void (*destroy)(void *), size_t capacity);
void destroy_map(map_t *map);
void add_list_item(list_t *list, void *value);
list_t *empty_list(void (*destroy)(void *), size_t capacity);
void destroy_list(list_t *list);
string_t *cstring_to_string(const char *cstring);
char *string_to_cstring(const string_t *string);
int hash_string(const string_t *string);
string_t *copy_string(const string_t *string);
string_t *merge_strings(const string_t *left, const string_t *right);
void extend_string_by_cstring(string_t *origin, const char *extension);
void extend_string_by_string(string_t *origin, const string_t *extension);
int compare_strings(const string_t *left, const string_t *right);
int is_keyword_match(const string_t *left, const char *right);
string_t *empty_string();
string_t *create_string(char *bytes, size_t length);
void destroy_string(string_t *string);
int add_numbers(number_t left, number_t right, number_t *out);
int subtract_numbers(number_t left, number_t right, number_t *out);
int multiply_numbers(number_t left, number_t right, number_t *out);
int divide_numbers(number_t left, number_t right, number_t *out);
int modulo_numbers(number_t left, number_t right, number_t *out);
int string_to_number(const string_t *text, number_t *out);
int integer_to_number(int integer, number_t *out);
int number_to_integer(number_t number, int *out);
string_t *represent_number(number_t number);
number_t truncate_number(number_t number);
int add_with_overflow(int left, int right);
int can_fit_both(size_t left, size_t right);
void *allocate(size_t number, size_t size);
void *allocate_with_zeros(size_t number, size_t size);
void *reallocate(void *memory, size_t number, size_t size);
void crash_with_message(const char *format, ...);

#endif
