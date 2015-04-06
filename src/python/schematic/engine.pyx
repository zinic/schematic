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
    engine.dyn_invoke = <dynamic_evaluator *> &dynamic_invoke

    return engine


cdef Instance * engine_leval(Engine *engine, List *code, Frame *frame):
    cdef const Instance *instruction = <Instance *> code.head.value
    return engine_eval(engine, instruction, code.head.next, frame)


cdef void print_instance_list(Node *head):
    cdef:
        Node *cursor = head
        Instance *inst

    while cursor != EMPTY_NODE:
        inst = <Instance *> cursor.value

        if inst.type == TYPE_LIST:
            print_instance_list((<List *> inst.data).head)
        else:
            instance_print(inst)
            printf(' ')

        cursor = cursor.next


cdef Instance * resolve(Engine *engine, Node *arg, Frame *frame):
    cdef:
        Instance *inst_cursor = <Instance *> arg.value

    while inst_cursor != NULL:
         if inst_cursor.type == TYPE_LIST:
             inst_cursor = engine_leval(engine, <List *> inst_cursor.data, frame)

         elif inst_cursor.type == TYPE_SYMBOL:
             inst_cursor = engine_eval(engine, inst_cursor, EMPTY_NODE, frame)

         else:
             break

    return inst_cursor


cdef List * resolve_all(Engine *engine, Node *vargs, Frame *frame):
    cdef:
        List *args = list_new()
        Node *arg_cursor = vargs

    while arg_cursor != EMPTY_NODE:
        inst_cursor = resolve(engine, arg_cursor, frame)
        list_append(args, inst_cursor)

        arg_cursor = arg_cursor.next

    return args


cdef Instance * engine_eval(Engine *engine, Instance *instruction, Node *args, Frame *frame):
    cdef:
        Instance *result = instruction
        Node *last

    if instruction != NULL:
        if instruction.type == TYPE_SYMBOL:
            result = frame_search(frame, <char *> instruction.data)

            if result == NULL:
                printf('Undefined symbol: %s\n', instruction.data)
            else:
#                printf('::fcall %s(', instruction.data)
#                print_instance_list(args)
#                printf(')\n')

                if result.type == TYPE_NATIVE_FUNC:
                    result = (<evaluator> result.data)(engine, args, frame)

                elif result.type == TYPE_DYNAMIC_FUNC:
                    result = (<dynamic_evaluator> engine.dyn_invoke)(
                        engine,
                        <DynamicFunction *> result.data,
                        args,
                        frame)

    return result


cdef Instance * dynamic_invoke(Engine *engine, DynamicFunction *fdef, Node *vargs, Frame *frame):
    cdef:
        Scope *fun_scope

        List *args
        Node *arg_cursor
        Node *arg_def_cursor

        char *arg_name
        Instance *arg_value
        Instance *retval = NULL

    fun_scope = scope_new()

    args = resolve_all(engine, vargs, frame)
    arg_cursor = args.head

    arg_def_cursor = (<List *> fdef.arg_defs.data).head

    while arg_cursor != EMPTY_NODE:
        arg_name = <char *> arg_def_cursor.value.data
        arg_def_cursor = arg_def_cursor.next

        arg_value = arg_cursor.value

        #printf('Binding %s to ', arg_name)
        #instance_print(arg_value)
        #printf('\n')

        arg_cursor = arg_cursor.next

        scope_add(fun_scope, arg_name, arg_value)

    frame_push(frame, fun_scope)
    retval = engine_leval(engine, (<List *> fdef.code.data), frame)

    frame_pop(frame)
    scope_free(fun_scope)

    return retval
