#include "interpreter.h"
#include "types.h"

#include <uthash.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

DynamicFunction * DynamicFunction_new(Box *symbol, List *parameter_defs, List *code) {
    DynamicFunction *new_func = (DynamicFunction *) malloc(sizeof(DynamicFunction));

    new_func->symbol = symbol;
    new_func->code = code;
    new_func->parameter_defs = parameter_defs;

    return new_func;
}

void DynamicFunction_free(DynamicFunction *dyn_func) {
    free(dyn_func);
}

BoxScope * BoxScope_new(String *name, Box *box) {
    BoxScope *new_sbox = (BoxScope *) malloc(sizeof(BoxScope));

    new_sbox->id= String_hash_code(name);
    new_sbox->box = box;

    return new_sbox;
}

void BoxScope_free(BoxScope *sbox) {
    free(sbox);
}

Frame * Frame_new() {
    Frame *new_frame = (Frame *) malloc(sizeof(Frame));
    new_frame->scope_map = NULL;

    return new_frame;
}

void Frame_put(Frame *frame, String *scope_name, Box *box) {
    BoxScope *new_scope = BoxScope_new(scope_name, box);

    HASH_ADD_INT(
        frame->scope_map,
        id,
        new_scope);
}

Box * Frame_get(Frame *frame, String *scope_name) {
    const int sname_hash = String_hash_code(scope_name);

    BoxScope *sbox;

    HASH_FIND_INT(
        frame->scope_map,
        &sname_hash,
        sbox);

    return sbox != NULL ? sbox->box : NULL;
}

void Frame_free(Frame *frame) {
    BoxScope *sbox, *tmp;

    HASH_ITER(hh, frame->scope_map, sbox, tmp) {
        HASH_DEL(frame->scope_map, sbox);
        BoxScope_free(sbox);
    }
}

Scope * Scope_new() {
    Scope *new_scope = (Scope *) malloc(sizeof(Scope));
    new_scope->frames = List_new();

    return new_scope;
}

Box * Scope_get(Scope *scope, String *name) {
    Frame *current_frame;
    Node *cursor = List_tail(scope->frames);
    Box *value = NULL;

    while (cursor != EMPTY_NODE && value == NULL) {
        current_frame = (Frame *) Node_retreat(&cursor);
        value = Frame_get(current_frame, name);
    }

    return value;
}

void Scope_put(Scope *scope, String *scope_name, Box *box) {
    Frame *local_frame = (Frame *) List_tail(scope->frames)->value;
    Frame_put(local_frame, scope_name, box);
}

Frame * Scope_descend(Scope *scope) {
    Frame *new_frame = Frame_new();
    List_append(scope->frames, new_frame);

    return new_frame;
}

void Scope_ascend(Scope *scope) {
    Frame *lost_frame = (Frame *) List_pop(scope->frames);
    Frame_free(lost_frame);
}

void Scope_free(Scope *scope) {
    Node *cursor = List_head(scope->frames);

    while (cursor != EMPTY_NODE) {
        Frame_free((Frame *) cursor->value);
    }

    List_free(scope->frames);
    free(scope);
}

Box * resolve(Box *instruction, Scope *local) {
    Box *retval = instruction;

    if (instruction->type == TYPE_SYMBOL) {
        retval = Scope_get(local, (String *) instruction->data);

        if (retval == NULL) {
            printf("Undefined symbol: %s\n", UNBOX(String, instruction)->data);
            exit(1);
        }
    } else if (instruction->type == TYPE_LIST) {
        retval = evaluate(UNBOX(List, instruction), local);
    }

    return retval;
}

Box * _sch_native_call(NativeFunction native_func, Node *args, Scope *local) {
    return (*native_func)(args, local);
}

Box * _sch_dynamic_call(DynamicFunction *function_def, Node *args, Scope *local) {
    Frame *function_scope_frame = Scope_descend(local);
    Node *param_def_cursor = function_def->parameter_defs->head;
    Node *param_cursor = args;
    Box *retval = NULL;

    while (param_def_cursor != EMPTY_NODE) {
        String *name = UNBOX(String, Node_advance(&param_def_cursor));
        Box *value = resolve((Box *) Node_advance(&param_cursor), local);
        
        Frame_put(function_scope_frame, name, value);
    }
    
    retval = evaluate(function_def->code, local);
    
    // Leave the function's scope
    Scope_ascend(local);
    
    return retval;
}

Box * evaluate(List *code, Scope *local) {
    const Node *list_head = code->head;
    const Box *instruction = resolve((Box *) list_head->value, local);
    Box* retval = NULL;

    if (instruction != NULL) {
        if (instruction->type == TYPE_NATIVE_FUNC) {
            retval = _sch_native_call(
                (NativeFunction) instruction->data,
                list_head->next,
                local);
        } else if (instruction->type == TYPE_DYNAMIC_FUNC) {
            retval = _sch_dynamic_call(
                UNBOX(DynamicFunction, instruction),
                list_head->next,
                local);
        } else {
            printf("Instruction is not callable!\n");
            exit(1);
        }
    }

    return retval;
}
