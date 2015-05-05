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

FrameBinding * BoxScope_new(String *name, Box *box) {
    FrameBinding *new_sbox = (FrameBinding *) malloc(sizeof(FrameBinding));

    new_sbox->id= String_hash_code(name);
    new_sbox->box = box;

    return new_sbox;
}

void BoxScope_free(FrameBinding *sbox) {
    free(sbox);
}

Frame * Frame_new() {
    Frame *new_frame = (Frame *) malloc(sizeof(Frame));
    new_frame->scope_map = NULL;

    return new_frame;
}

void Frame_put(Frame *frame, String *scope_name, Box *box) {
    FrameBinding *new_scope = BoxScope_new(scope_name, box);

    HASH_ADD_INT(
        frame->scope_map,
        id,
        new_scope);
}

Box * Frame_get(Frame *frame, String *scope_name) {
    const int sname_hash = String_hash_code(scope_name);
    FrameBinding *sbox;

    HASH_FIND_INT(
        frame->scope_map,
        &sname_hash,
        sbox);

    return sbox != NULL ? sbox->box : NULL;
}

Box * Frame_remove(Frame *frame, String *scope_name) {
    const int sname_hash = String_hash_code(scope_name);
    FrameBinding *sbox;
    Box *value = NULL;

    HASH_FIND_INT(
        frame->scope_map,
        &sname_hash,
        sbox);
    
    if (sbox != NULL) {
        value = sbox->box;
        
        HASH_DEL(frame->scope_map, sbox);
        BoxScope_free(sbox);
    }
        
    return value; 
}

void rfree(Type type, void *ptr) {
    if (type == TYPE_BOX) {
        Box *box = (Box *) ptr;
        rfree(box->type, box->data);
        
        free(box);
    } else if (type == TYPE_LIST) {
        List *lref = (List *) ptr;
        Node *cursor = lref->head;

        while (cursor != NULL) {
            rfree(TYPE_BOX, Node_advance(&cursor));
        }

        List_free(lref);
    } else if (type == TYPE_SYMBOL || type == TYPE_STRING) {
        String_free((String *) ptr);
    } else if (type == TYPE_NUMBER || type == TYPE_DYNAMIC_FUNC) {
        free(ptr);
    } else if (type != TYPE_NATIVE_FUNC) {
        printf("Unable to free type %s!\n", Type_name(type));
    }
}

void rfree_box(Box *box) {
    rfree(TYPE_BOX, box);
}

void Frame_free(Frame *frame) {
    FrameBinding *sbox, *tmp;

    HASH_ITER(hh, frame->scope_map, sbox, tmp) {
        HASH_DEL(frame->scope_map, sbox);
        
        // Free the inner box
        rfree_box(sbox->box);
        
        // Free the scope holder
        BoxScope_free(sbox);
    };
    
    free(frame);
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

    while (cursor != NULL && value == NULL) {
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
    Node *cursor = scope->frames->head;

    while (cursor != NULL) {
        Frame_free((Frame *) Node_advance(&cursor));
    }

    List_free(scope->frames);
    free(scope);
}

List * resolve_args(Node *args, Scope *local) {
    List *resolved_args = List_new();
    Node *cursor = args;
    Box *arg_box;

    while (cursor != NULL) {
        Resolution *resolution = (Resolution *) malloc(sizeof(Resolution));
        
        // Resolutions are not reclaimable by default
        resolution->reclaimable = false;
        
        // Next argument box
        arg_box = (Box *) Node_advance(&cursor);

        // Resolve if need be
        if (arg_box->type == TYPE_SYMBOL) {
            resolution->box = resolve(arg_box, local);
        } else if (arg_box->type == TYPE_LIST) {
            resolution->box = evaluate(UNBOX(List, arg_box), local);
            resolution->reclaimable = resolution->box != NULL;
        } else {
            resolution->box = arg_box;
        }
        
        List_append(resolved_args, resolution);
    }
    
    return resolved_args;
}

void free_resolved_args(List *resolved_args) {
    Node *cursor = resolved_args->head;
    
    while (cursor != NULL) {
        Resolution *resolution = (Resolution *) Node_advance(&cursor);
        
        if (resolution->reclaimable) {
            // Free the box associated with this resolution
            rfree_box(resolution->box);
        }
        
        free(resolution);
    }
    
    List_free(resolved_args);
}

Box * resolve(Box *instruction, Scope *local) {
    Box *retval = Scope_get(local, (String *) instruction->data);

    if (retval == NULL) {
        printf("Undefined symbol: %s\n", UNBOX(String, instruction)->data);
        exit(1);
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

    while (param_def_cursor != NULL) {
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
