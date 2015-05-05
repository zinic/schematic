#include <stdlib.h>

#include "structures.h"

void cstr_copy(char *dest, const char *source, size_t num) {
    for (size_t idx = 0; idx < num; idx++) {
        dest[idx] = source[idx];
    }
}

size_t cstr_length(const char *str) {
    register const char *s;
    
    for (s = str; *s; ++s);
    return (s - str);
}

SNode * SNode_wrap(void *value) {
    SNode *new_node = (SNode *) malloc(sizeof(SNode));
    new_node->value = value;
    
    return new_node;
}

void SNode_free(SNode *node) {
    free(node);
}

SList * SList_new() {
    SList *new_slist = (SList *) malloc(sizeof(SList));
    new_slist->head = EMPTY_NODE;
    new_slist->tail = EMPTY_NODE;
    return new_slist;
}

void SList_free(SList *slist) {
    SNode *cursor = slist->head;
    SNode *free_target;
    
    while (cursor != EMPTY_NODE) {
        free_target = cursor;
        cursor = cursor->next;
        
        free(free_target);
    }
}

void SList_push(SList *list, void *value) {
    SNode *new_node = SNode_wrap(value);
    
    if (list->head == EMPTY_NODE) {
        list->head = new_node;
    } else if (list->tail == EMPTY_NODE) {
        list->tail = list->head;
        list->head = new_node;
        list->head->next = list->tail;
    } else {
        new_node->next = list->head;
        list->head = new_node;
    }
}

void SList_append(SList *list, void *value) {
    if (list->head == EMPTY_NODE) {
        list->head = SNode_wrap(value);
    } else if (list->tail == EMPTY_NODE) {
        list->tail = SNode_wrap(value);
        list->head->next = list->tail;
    } else {
        list->tail->next = SNode_wrap(value);
        list->tail = list->tail->next;
    }
}

void * SList_head(SList *list) {
    void *value = NULL;
    
    if (list->head != EMPTY_NODE) {
        value = list->head->value;
    }
    
    return value;
}

void * SList_remove_head(SList *list) {
    SNode *head = list->head;
    void *value = NULL;
    
    if (head != EMPTY_NODE) {
        value = head->value;
        list->head = head->next;
        
        SNode_free(head);
        
    }
    
    return value;
}

DNode * DNode_wrap(void *value) {
    DNode *new_node = (DNode *) malloc(sizeof(DNode));
    new_node->next = EMPTY_NODE;
    new_node->previous = EMPTY_NODE;
    new_node->value = value;
    return new_node;
}

void DNode_link(DNode *previous, DNode *next) {
    previous->next = next;
    next->previous = previous;
}

void DNode_free(DNode *node) {
    free(node);
}

DList * DList_new() {
    DList *list = (DList *) malloc(sizeof(DList));
    list->head = EMPTY_NODE;
    list->tail = EMPTY_NODE;
    return list;
}

DNode * DList_head(DList *list) {
    return list->head;
}

DNode * DList_tail(DList *list) {
    return list->tail != NULL ? list->tail : list->head;
}

void DList_append(DList *list, void *value) {
    DNode *new_node = DNode_wrap(value);

    if (list->head == NULL) {
        list->head = new_node;
    } else if(list->tail == NULL) {
        list->tail = new_node;
        DNode_link(list->head, list->tail);
    } else {
        DNode_link(list->tail, new_node);
        list->tail = new_node;
    }
}

void * DList_pop(DList *list) {
    void *value = NULL;
    DNode *node_ref;

    if (list->head != NULL) {
        if (list->tail != NULL) {
            // Grab the node and the value
            node_ref = list->tail;
            value = node_ref->value;

            if (list->head == list->tail->previous) {
                list->tail = NULL;
                list->head->next = NULL;
            } else {
                list->tail = list->tail->previous;
                list->tail->next = NULL;
            }

            DNode_free(node_ref);
        } else {
            value = list->head->value;
            DNode_free(list->head);

            list->head = NULL;
        }
    }

    return value;
}

void DList_free(DList *list) {
    DNode *cursor = list->head, *free_target;

    while (cursor != NULL) {
        free_target = cursor;
        cursor = cursor->next;

        DNode_free(free_target);
    }

    free(list);
}

String * String_new(size_t size) {
    String *new_string = (String *) malloc(sizeof(String));
    char *data = (char *) malloc(sizeof(char) * size);

    new_string->size = size;
    new_string->data = data;

    return new_string;
}

String * String_concat(String *front, String *back) {
    String *new_str = String_new(front->size + back->size);
    
    cstr_copy(new_str->data, front->data, front->size);
    cstr_copy(new_str->data + front->size, back->data, back->size);
    
    return new_str;
}

void String_resize(String *string, size_t new_size) {
    string->data = realloc(string->data, sizeof(char) * new_size);
    string->size = new_size;
}

String * String_wrap(char *data) {
    String *new_string = (String *) malloc(sizeof(String));

    new_string->size = cstr_length(data);
    new_string->data = data;

    return new_string;
}

String * String_wrapn(char *data, size_t size) {
    String *new_string = (String *) malloc(sizeof(String));

    new_string->size = size;
    new_string->data = data;

    return new_string;
}


int String_hash_code(String *string) {
    int code = 15485863;

    for (int i = 0; i < string->size; i++) {
        code += string->data[i];
    }

    return code;
}

void String_free(String *string) {
    free(string->data);
    free(string);
}