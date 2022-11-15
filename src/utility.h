#ifndef UTILITY_H
#define UTILITY_H

#define PROGRAM_VERSION "v0.24.2"
#define PROGRAM_NAME "langnd"
#define PROGRAM_SUCCESS 0
#define PROGRAM_FAILURE 1

#define PORTABLE_INT_LIMIT 2147483647
#define PORTABLE_LONG_LIMIT 9223372036854775807

typedef enum
{
    VALUE_TYPE_NULL = 0x01,
    VALUE_TYPE_BOOLEAN = 0x02,
    VALUE_TYPE_NUMBER = 0x04,
    VALUE_TYPE_STRING = 0x08,
    VALUE_TYPE_LIST = 0x10
} value_type_t;

typedef struct
{
    value_type_t type;
    void *data;
    int thrown;
    int owners;
} value_t;

typedef struct map_chain_t
{
    char *key;
    void *value;
    struct map_chain_t *next;
} map_chain_t;

typedef struct
{
    int (*hash)(char *);
    void (*destroy)(void *);
    size_t length;
    size_t capacity;
    map_chain_t **chains;
} map_t;

typedef struct
{
    void (*destroy)(void *);
    size_t capacity;
    size_t length;
    void **items;
} list_t;

/**
 * 16.16 signed fixed point number.
 */
typedef int number_t;

typedef char boolean_t;
static const boolean_t TRUE = 1;
static const boolean_t FALSE = 0;

void assure_portable_environment();
int compare_values(value_t *left, value_t *right);
char *represent_value(value_t *value);
value_t *throw_error(char *message);
value_t *new_null();
value_t *new_boolean(boolean_t boolean);
value_t *new_number(number_t number);
value_t *new_string(char *string);
value_t *steal_string(char *string);
value_t *steal_list(list_t *list);
boolean_t view_boolean(value_t *value);
number_t view_number(value_t *value);
char *view_string(value_t *value);
list_t *view_list(value_t *value);
void destroy_value(value_t *value);
void dereference_value(value_t *value);
map_t *empty_map(int (*hash)(char *), void (*destroy)(void *), size_t capacity);
int has_map_item(map_t *map, char *key);
void *get_map_item(map_t *map, char *key);
void set_map_item(map_t *map, char *key, void *value);
void unset_map_item(map_t *map, char *key);
void destroy_map(map_t *map);
list_t *empty_list(void (*destroy)(void *));
void add_list_item(list_t *list, void *value);
void destroy_list(list_t *list);
int add_numbers(number_t left, number_t right, number_t *out);
int subtract_numbers(number_t left, number_t right, number_t *out);
int multiply_numbers(number_t left, number_t right, number_t *out);
int divide_numbers(number_t left, number_t right, number_t *out);
int modulo_numbers(number_t left, number_t right, number_t *out);
int string_to_number(char *text, number_t *out);
int integer_to_number(int integer, number_t *out);
int number_to_integer(number_t number, int *out);
char *represent_number(number_t number);
number_t truncate_number(number_t number);
int hash_string(char *string);
int add_with_overflow(int left, int right);
char *merge_strings(char *start, char *expansion);
char *copy_string(char *string);
void *allocate(size_t size);
void *allocate_with_zeros(size_t number, size_t size);
void *reallocate(void *memory, size_t size);
void crash_with_message(char *format, ...);

#endif
