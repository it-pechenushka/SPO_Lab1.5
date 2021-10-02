#include "linked_list.h"

#include <stddef.h>
#include <string.h>
#include <malloc.h>

uint32_t add_first(LinkedList *ptr, void *element) {
    Node_L *created = malloc(sizeof(Node_L));
    created->next = ptr->first;
    created->value = element;
    ptr->first->prev = created;
    ptr->first = created;
    return 0;
}

uint32_t add_last(LinkedList *ptr, void *element) {
    Node_L *created = malloc(sizeof(Node_L));
    bzero(created, sizeof(Node_L));
    created->prev = ptr->last;
    created->value = element;
    if (ptr->last == NULL) {
        ptr->last = created;
        ptr->first = created;
    } else {
        ptr->last->next = created;
        ptr->last = created;
    }
    return ++ptr->size;
}

LinkedList *init_list() {
    LinkedList *created = malloc(sizeof(LinkedList));
    created->first = NULL;
    created->last = NULL;
    created->size = 0;
    return created;
}

void free_list(LinkedList *ptr, bool is_malloc_value) {
    Node_L *current = ptr->first;
    if (ptr->size != 0) {
        for (uint64_t i = 1; i < ptr->size; ++i) {
            current = current->next;
            if (is_malloc_value) free(current->prev->value);
            free(current->prev);
        }
        if (is_malloc_value) free(current->value);
        free(current);
    }
    free(ptr);
}

void remove_first(LinkedList *list) {
    list->size--;
    Node_L *next_first = list->first->next;
    free(list->first);
    list->first = next_first;
}

void remove_element(bool (*by)(void *, char *, char *), LinkedList *ptr, char *first_to_find, char *second_to_find) {
    Node_L *current = ptr->first;
    while (current != NULL && !by(current->value, first_to_find, second_to_find)) {
        current = current->next;
    }
    if (current != NULL) {
        if (ptr->first == current) {
            ptr->first = current->next;
        }
        if (ptr->last == current) {
            ptr->last = current->prev;
        }
        if (current->prev != NULL) {
            current->prev->next = current->next;
        }
        if (current->next != NULL) {
            current->next->prev = current->prev;
        }
        ptr->size--;
        free(current);
    }
}

void *find_element(bool (*by)(void *, char *, char *), LinkedList *ptr, char *first_to_find, char *second_to_find) {
    if (ptr->size == 0) {
        return NULL;
    }
    Node_L *current = ptr->first;
    while (current != NULL && !by(current->value, first_to_find, second_to_find)) {
        current = current->next;
    }
    if (current == NULL) return NULL;
    return current->value;
}

bool by_value(void *value, char *to_find, char *second_argument) {
    return strcmp(value, to_find) == 0;
}

uint16_t get_last_n(LinkedList *ptr, void **buffer, uint16_t buffer_size, bool(*filter)(void *, char *to_filter),
                    char *to_filter) {
    Node_L *current = ptr->last;
    for (uint16_t i = 0; i < buffer_size; ++i) {
        if (current == NULL) return i;
        if (filter == NULL || filter(current->value, to_filter)) {
            buffer[i] = current->value;
        } else {
            --i;
        }
        current = current->prev;
    }
    return buffer_size;
}