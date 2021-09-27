#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

#include <stdint.h>
#include <stdbool.h>

typedef struct _Node {
    struct _Node *prev;
    void *value;
    struct _Node *next;
} Node;

typedef struct _Property {
    char key[20];
    char value[20];
} Property;

typedef struct _LinkedList {
    Node *first;
    Node *last;
    uint32_t size;

    void (*free_value_fun)(void *value);
} LinkedList;

void *get_element(LinkedList *ptr, uint32_t index);

uint32_t add_first(LinkedList *ptr, void *element);

uint32_t add_last(LinkedList *ptr, void *element);

bool by_value(void *value, char *to_find, char *second_argument);

bool by_key(void *value, char *key, char *second_value);

LinkedList *init_list();

void free_list(LinkedList *ptr, bool is_alloc_value);

void remove_element(bool (*by)(void *, char *, char *), LinkedList *ptr, char *first_to_find, char *second_to_find);

void *find_element(bool (*by)(void *, char *, char *), LinkedList *ptr, char *first_to_find, char *second_to_find);

uint16_t get_last_n(LinkedList *ptr, void **buffer, uint16_t buffer_size, bool(*filter)(void *, char *to_filter),
                    char *to_filter);

void remove_first(LinkedList *list);

#endif
