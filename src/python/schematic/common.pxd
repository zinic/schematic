# Node type hint type
ctypedef size_t Type


# Type constants
cdef Type TYPE_SYMBOL, TYPE_STRING, TYPE_NUMBER, TYPE_LIST, TYPE_NATIVE_FUNC, \
    TYPE_DYNAMIC_FUNC


# Bridge type for uthash macros
ctypedef void * C_VARNAME


# uthash defs
cdef extern from "uthash.h":

    ctypedef struct UT_hash_handle:
        pass

    cdef int HASH_COUNT(ScopedInstance *)
    cdef void HASH_FIND_INT(ScopedInstance *, int *, ScopedInstance *)
    cdef void HASH_ADD_INT(ScopedInstance *, C_VARNAME, ScopedInstance *)


# Instance structs
ctypedef struct Instance:
    void *data
    Type type


ctypedef struct ScopedInstance:
    Instance *instance
    char *name
    int h_id

    UT_hash_handle hh


ctypedef struct Scope:
    ScopedInstance *instances


cdef Scope * scope_new()
cdef void scope_add(Scope *, char *, Instance *)
cdef ScopedInstance * scope_get(Scope *, char *)
cdef void scope_free(Scope *)


# Runtime structs
ctypedef struct DynamicFunction:
    Instance *symbol
    Instance *arg_defs
    Instance *code


# List structs
ctypedef struct Node:
    Instance *value

    Node *next
    Node *previous


ctypedef struct ListIterator:
    Node *position


ctypedef struct List:
    Node *head
    Node *tail

    size_t length


# Empty constnats
cdef Instance *EMPTY_INSTANCE
cdef Node *EMPTY_NODE


# Helper functions
cdef int ptr_void_to_int(void *)


# Instance functions
cdef void instance_print(Instance *)

cdef Instance * instance_wrap_ptr(void *, Type)
cdef Instance * instance_wrap_int(int)

cdef void instance_free(Instance *)


# Node functions

cdef Node * node_new(Instance *)
cdef void node_free(Node *)


# List functions
cdef List * list_new()
cdef List * list_free(List *)
cdef int list_empty(List *)
cdef void list_append(List *, Instance *)
cdef Node * list_tail(List *)
cdef Instance * list_peek(List *)
cdef Instance * list_pop(List *)


# Accumulator type and functions
ctypedef struct Accumulator:
    char *buffer
    size_t write_idx


cdef Accumulator * accumulator_new()
cdef int accumulator_has_bytes(Accumulator *)
cdef void accumulator_free(Accumulator *)
cdef void accumulator_append(Accumulator *, char)
cdef char * accumulator_get(Accumulator *)
