#include "interpreter.h"
#include "types.h"

#include <uthash.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// [print x "a" 421]
Box * print_fn(Node *args, Scope *local) {
    Node *cursor = args;
    Box *ref;

    while (cursor != EMPTY_NODE) {
        ref = resolve((Box *) Node_advance(&cursor), local);

        if (ref->type == TYPE_STRING) {
            printf("%s", UNBOX(String, ref)->data);
        } else if (ref->type == TYPE_NUMBER) {
            printf("%f", *UNBOX(double, ref));
        } else {
            printf("Unprintable!");
        }
    }

    return NULL;
}

// [def f [arg1 arg2] [do arg1 arg2]]
Box * define_fn(Node *args, Scope *local) {
    Node *cursor = args;
    
    Box *name = (Box *) Node_advance(&cursor);
    List *arg_defs = UNBOX(List, Node_advance(&cursor));
    List *code = UNBOX(List, Node_advance(&cursor));

    DynamicFunction *new_function_def = DynamicFunction_new(
        name, arg_defs, code);

    Scope_put(local, UNBOX(String, name), Box_wrap(TYPE_DYNAMIC_FUNC, new_function_def));

    return NULL;
}

void load_std_grammar(Frame *frame) {
    Frame_put(frame, AS_STRING("def"), Box_wrap(TYPE_NATIVE_FUNC, &define_fn));
    Frame_put(frame, AS_STRING("print"), Box_wrap(TYPE_NATIVE_FUNC, &print_fn));
}
