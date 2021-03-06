#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>

const char * Type_name(Type type) {
    return TYPE_NAMES[type];
}

void print_boxed_list(List *list) {
    Node *cursor = list->head;
    Box *box;

    printf("[");

    while (cursor != NULL) {
        box = (Box *) Node_advance(&cursor);

        if (box->type == TYPE_NULL) {
            printf("null ");
        } else if (box->type == TYPE_LIST) {
            print_boxed_list(UNBOX(List, box));
        } else if (box->type == TYPE_SYMBOL) {
            printf("smybol:%s", UNBOX(String, box)->data);
        } else if (box->type == TYPE_STRING) {
            printf("\"%s\"", UNBOX(String, box)->data);
        } else if (box->type == TYPE_NUMBER) {
            printf("number:%f", *UNBOX(double, box));
        }

        if (cursor != NULL) {
            printf(" ");
        }
    }

    printf("]");
}

Box * Box_new() {
    Box *new_box = (Box *) malloc(sizeof(Box));
    new_box->type = TYPE_NULL;
    new_box->data = NULL;
    return new_box;
}

Box * Box_wrap(Type type, void *data) {
    Box *new_box = Box_new();
    new_box->type = type;
    new_box->data = data;
    return new_box;
}

Box * Box_wrap_double(double data) {
    double *data_ptr = (double *) malloc(sizeof(double));
    *data_ptr = data;

    return Box_wrap(TYPE_NUMBER, data_ptr);
}

void Box_free(Box *box) {
    free(box);
}

Node * Node_new() {
    Node *new_node = (Node *) malloc(sizeof(Node));

    new_node->value = NULL;
    new_node->next = NULL;
    new_node->previous = NULL;

    return new_node;
}

void * Node_advance(Node **node_ref) {
    const Node *node = *node_ref;
    void *value = node->value;

    *node_ref = node->next;

    return value;
}

void * Node_retreat(Node **node_ref) {
    const Node *node = *node_ref;
    void *value = node->value;

    *node_ref = node->previous;

    return value;
}


void Node_link(Node *previous, Node *next) {
    previous->next = next;
    next->previous = previous;
}

void Node_free(Node *node) {
    free(node);
}

List * List_new() {
    List *new_list = (List *) malloc(sizeof(List));

    new_list->size = 0;
    new_list->head = NULL;
    new_list->tail = NULL;

    return new_list;
}

Node * List_head(List *list) {
    return list->head;
}

Node * List_tail(List *list) {
    return list->tail != NULL ? list->tail : list->head;
}

void List_append(List *list, void *value) {
    Node *new_node = Node_new();
    new_node->value = value;

    list->size += 1;

    if (list->head == NULL) {
        list->head = new_node;
    } else if(list->tail == NULL) {
        list->tail = new_node;
        Node_link(list->head, list->tail);
    } else {
        Node_link(list->tail, new_node);
        list->tail = new_node;
    }
}

void * List_pop(List *list) {
    void *value = NULL;
    Node *node_ref;

    if (list->head != NULL) {
        list->size -= 1;

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

            Node_free(node_ref);
        } else {
            value = list->head->value;
            Node_free(list->head);

            list->head = NULL;
        }
    }

    return value;
}

void List_free(List *list) {
    Node *cursor = list->head, *free_target;

    while (cursor != NULL) {
        free_target = cursor;
        cursor = cursor->next;

        Node_free(free_target);
    }

    free(list);
}

Buffer * Buffer_new(size_t max_size) {
    Buffer *buffer = (Buffer *) malloc(sizeof(Buffer));

    buffer->data = (char *) malloc(sizeof(char) * max_size);
    buffer->max_size = max_size;
    buffer->write_idx = 0;

    return buffer;
}

void Buffer_free(Buffer *buffer) {
    free(buffer->data);
    free(buffer);
}

void Buffer_append(Buffer *buffer, char ch) {
    buffer->data[buffer->write_idx] = ch;
    buffer->write_idx += 1;
}

bool Buffer_has_data(Buffer *buffer) {
    return buffer->write_idx > 0;
}

String * Buffer_get(Buffer *buffer) {
    int size = buffer->write_idx + 1, size_bytes = sizeof(char) * size;
    char *value = (char *) malloc(size_bytes);

    // Blank the array so that null-term strings ping correctly among other
    // things like uninitialized junk.
    memset(value, 0, size_bytes);

    // Get the goods in minus the last null cell
    strncpy(value, buffer->data, (size - 1));

    // Reset the buffer
    buffer->write_idx = 0;

    // The new string assumes ownership of the newly allocated char array
    return String_wrapn(value, size);
}
