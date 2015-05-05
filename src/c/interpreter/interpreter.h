#ifndef interpreter_h
#define interpreter_h

#ifdef __cplusplus
extern "C" {
#endif

#include <uthash.h>
#include "types.h"

#define RES_UNBOX(type, resolution) UNBOX(type, ((Resolution *) resolution)->box)
    
#define with_resolved_args(args, local, cursor, ...)\
    List *__ra__ = resolve_args(args, local);\
    cursor = __ra__->head;\
    __VA_ARGS__\
    free_resolved_args(__ra__);


// Dynamic function defs
typedef struct DynamicFunction DynamicFunction;

struct DynamicFunction {
    Box *symbol;

    List *code;
    List *parameter_defs;
};

DynamicFunction * DynamicFunction_new(Box *symbol, List *parameter_defs, List *code);
void DynamicFunction_free(DynamicFunction *dyn_func);

void rfree(Type type, void *ptr);
void rfree_box(Box *box);

// Type boxing
typedef struct FrameBinding FrameBinding;

struct FrameBinding {
    Box *box;
    int id;

    UT_hash_handle hh;
};

FrameBinding * BoxScope_new(String *name, Box *box);


// Frame defs
typedef struct Frame Frame;

struct Frame {
    FrameBinding *scope_map;
};

Frame * Frame_new();
void Frame_put(Frame *frame, String *name, Box *box);
Box * Frame_get(Frame *frame, String *name);
Box * Frame_remove(Frame *frame, String *name);
void Frame_free(Frame *frame);


// Scope defs
typedef struct Scope Scope;

struct Scope {
    List *frames;
};

Scope * Scope_new();

Box * Scope_get(Scope *scope, String *name);
void Scope_put(Scope *scope, String *scope_name, Box *box);
Frame * Scope_descend(Scope *scope);
void Scope_ascend(Scope *scope);
void Scope_free(Scope *scope);


// Resolution management
typedef struct Resolution Resolution;

struct Resolution {
    Box *box;
    bool reclaimable;
};

List * resolve_args(Node *args, Scope *local);
void free_resolved_args(List *resolved_args);


// Core functions
typedef Box * (*NativeFunction)(Node *args, Scope *local);

Box * resolve(Box *instruction, Scope *local);
Box * evaluate(List *code, Scope *local);

#ifdef __cplusplus
}
#endif
#endif
