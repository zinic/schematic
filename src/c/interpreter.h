#ifndef interpreter_h
#define interpreter_h

#ifdef __cplusplus
extern "C" {
#endif

#include <uthash.h>
#include "types.h"


// Dynamic function defs
typedef struct DynamicFunction DynamicFunction;

struct DynamicFunction {
    Box *symbol;

    List *code;
    List *parameter_defs;
};

DynamicFunction * DynamicFunction_new(Box *symbol, List *parameter_defs, List *code);
void DynamicFunction_free(DynamicFunction *dyn_func);


// Type boxing
typedef struct BoxScope BoxScope;

struct BoxScope {
    Box *box;
    int id;

    UT_hash_handle hh;
};

BoxScope * BoxScope_new(String *name, Box *box);


// Frame defs
typedef struct Frame Frame;

struct Frame {
    BoxScope *scope_map;
};

Frame * Frame_new();
void Frame_put(Frame *frame, String *name, Box *box);
Box * Frame_get(Frame *frame, String *name);
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


// Core functions
typedef Box * (*NativeFunction)(Node *args, Scope *local);

Box * resolve(Box *instruction, Scope *local);
Box * evaluate(List *code, Scope *local);

#ifdef __cplusplus
}
#endif
#endif
