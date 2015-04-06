from libc.stdio cimport printf
from libc.stdlib cimport malloc, free

from schematic.common cimport *
from schematic.constants cimport *


cdef Type FRAME_SCOPE = 50


cdef Frame * frame_new():
    cdef Frame *new_frame = <Frame *> malloc(sizeof(Frame))
    new_frame.scope_stack = list_new()

    return new_frame


cdef void frame_push(Frame *frame, Scope *scope):
    list_append(frame.scope_stack, instance_wrap_ptr(scope, FRAME_SCOPE))


cdef Scope * frame_peek(Frame *frame):
    return <Scope *> list_peek(frame.scope_stack).data


cdef void frame_pop(Frame *frame):
    cdef const Instance *frame_scope_inst = list_pop(frame.scope_stack)
    instance_free(frame_scope_inst)


cdef Instance * frame_search(Frame *frame, char *name):
    cdef:
        Node *cursor = list_tail(frame.scope_stack)
        Scope *scope_cursor
        ScopedInstance *scoped_inst

    while cursor != EMPTY_NODE:
        scope_cursor = <Scope *> cursor.value.data
        scoped_inst = scope_get(scope_cursor, name)

        if scoped_inst != NULL:
            return scoped_inst.instance

        cursor = cursor.previous

    return NULL


cdef Engine * engine_new():
    cdef Engine *engine = <Engine *> malloc(sizeof(Engine))
    engine.root_frame = frame_new()

    return engine


cdef Instance * engine_leval(Engine *engine, List *code, Frame *frame):
    cdef const Instance *instruction = <Instance *> code.head.value
    return engine_eval(engine, instruction, code.head.next, frame)


cdef Instance * engine_eval(Engine *engine, Instance *instruction, Node *args, Frame *frame):
    cdef:
        Instance *result = instruction

    if instruction != NULL:
        if instruction.type == TYPE_SYMBOL:
            result = frame_search(frame, <char *> instruction.data)

            if result == NULL:
                printf('Undefined symbol: %s\n', instruction.data)
            else:
                if result.type == TYPE_NATIVE_FUNC:
                    result = (<evaluator> result.data)(engine, args, frame)

                elif result.type == TYPE_DYNAMIC_FUNC:
                    result = (<dynamic_evaluator> engine.dyn_invoke)(
                        engine,
                        <DynamicFunction *> result.data,
                        args,
                        frame)

    return result
