#ifndef UTILITY_H
#define UTILITY_H

#define PROGRAM_VERSION "v0.0.0"
#define PROGRAM_NAME "langnd"
#define PROGRAM_SUCCESS 0
#define PROGRAM_FAILURE 1

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

map_t *empty_map(int (*hash)(char *), void (*destroy)(void *), size_t capacity);
int has_map_item(map_t *map, char *key);
void *get_map_item(map_t *map, char *key);
void set_map_item(map_t *map, char *key, void *value);
void remove_map_item(map_t *map, char *key);
void destroy_map(map_t *map);
int hash_string(char *string);
int add_with_overflow(int left, int right);
char *copy_string(char *string);
void *allocate(size_t size);
void *allocate_with_zeros(size_t number, size_t size);
void crash_with_message(char *format, ...);

#endif
