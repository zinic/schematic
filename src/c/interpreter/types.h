#ifndef types_h
#define types_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>

#include "structures.h"
    
#define UNBOX(type, box) ((type *) ((Box *)box)->data)

// Type defs
typedef unsigned int Type;

static const Type TYPE_NULL = 0;
static const Type TYPE_LIST = 1;
static const Type TYPE_SYMBOL = 2;
static const Type TYPE_STRING = 3;
static const Type TYPE_NUMBER = 4;
static const Type TYPE_NATIVE_FUNC = 5;
static const Type TYPE_DYNAMIC_FUNC = 6;
static const Type TYPE_BOX = 7;

static const char * const TYPE_NAMES[] = {
    "null", "list", "symbol", "string", "number", "native_func", "dynamic_func", "box"
};

const char * Type_name(Type type);


// List defs
typedef struct Node Node;
typedef struct List List;

struct Node {
    void *value;

    Node *next;
    Node *previous;
};

struct List {
    size_t size;

    Node *head;
    Node *tail;
};


void * Node_advance(Node **node_ref);
void * Node_retreat(Node **node_ref);

List * List_new();
Node * List_head(List *list);
Node * List_tail(List *list);
void List_append(List *list, void *value);
void * List_pop(List *list);
void List_free(List *list);


// Buffer defs
typedef struct Buffer Buffer;

struct Buffer {
    char *data;

    size_t max_size;
    size_t write_idx;
};

Buffer * Buffer_new(size_t max_size);
void Buffer_free(Buffer *buffer);
void Buffer_append(Buffer *buffer, char ch);
bool Buffer_has_data(Buffer *buffer);
String * Buffer_get(Buffer *buffer);


// Boxing defs
typedef struct Box Box;

struct Box {
    Type type;
    void *data;
};

Box * Box_new();
Box * Box_wrap(Type type, void *data);
Box * Box_wrap_double(double data);
void Box_free(Box *box);


// Helpers functions
void print_boxed_list(List *list);

#ifdef __cplusplus
}
#endif
#endif
