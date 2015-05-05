/* 
 * File:   structures.h
 * Author: zinic
 *
 * Created on May 4, 2015, 9:22 PM
 */

#ifndef STRUCTURES_H
#define	STRUCTURES_H

#ifdef	__cplusplus
extern "C" {
#endif

#define EMPTY_NODE NULL
    
// Singly linked variants
typedef struct SNode SNode;

struct SNode {
    SNode *next;
    void *value;
};

struct SList {
    SNode *head;
    SNode *tail;
};

typedef struct SList SList;
typedef SList Queue;
typedef SList Stack;

SList * SList_new();
void SList_free(SList *slist);
void SList_push(SList *list, void *value);
void SList_append(SList *list, void *value);
void * SList_head(SList *list);
void * SList_remove_head(SList *list);

#define Queue_new() (Queue *) SList_new()
#define Queue_free(queue) SList_free(queue)
#define Queue_append(queue, value) SList_append(queue, value)
#define Queue_poll(queue) SList_remove_head(queue)
#define Queue_peek(queue) SList_head(queue)

#define Stack_new() (Stack *) SList_new()
#define Stack_free(stack) SList_free(stack)
#define Stack_push(stack, value) SList_push(stack, value)
#define Stack_pop(stack) SList_remove_head(stack)
#define Stack_peek(stack) SList_head(stack)

// Doubly linked variants
typedef struct DNode DNode;

struct DNode {
    DNode *next;
    DNode *previous;
    void *value;
};

typedef struct DList {
    DNode *head;
    DNode *tail;
} DList;

// Everyone does it
typedef struct String String;

struct String {
    size_t size;
    char *data;
};

String * String_new(size_t size);
String * String_concat(String *front, String *back);
String * String_wrap(char *data);
String * String_wrapn(char *data, size_t size);
void String_resize(String *string, size_t new_size);
int String_hash_code(String *string);
void String_free(String *string);

#ifdef	__cplusplus
}
#endif

#endif	/* STRUCTURES_H */

