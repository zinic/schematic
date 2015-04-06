from libc.stdio cimport printf
from libc.stdlib cimport malloc, free

from schematic.common cimport *
from schematic.engine cimport *
from schematic.constants cimport *


cdef Scope * std_grammar():
    cdef Scope *grammar = scope_new()

    scope_add(grammar, '+', instance_wrap_ptr(
        &__accumulate, TYPE_NATIVE_FUNC))

    scope_add(grammar, 'def', instance_wrap_ptr(
        &__define_function, TYPE_NATIVE_FUNC))

    scope_add(grammar, 'print', instance_wrap_ptr(
        &__print_function, TYPE_NATIVE_FUNC))

    return grammar


cdef List * __resolve(Engine *engine, Node *vargs, Frame *frame):
    cdef:
        List *args
        Node *arg_cursor
        Instance *inst_cursor

    args = list_new()

    arg_cursor = vargs
    while arg_cursor != EMPTY_NODE:
        inst_cursor = <Instance *> arg_cursor.value

        while inst_cursor != NULL:
            if inst_cursor.type == TYPE_LIST:
                inst_cursor = engine_leval(engine, <List *> inst_cursor.data, frame)

            elif inst_cursor.type == TYPE_SYMBOL:
                inst_cursor = engine_eval(engine, inst_cursor, EMPTY_NODE, frame)

            else:
                list_append(args, inst_cursor)
                inst_cursor = NULL

        arg_cursor = arg_cursor.next

    return args


cdef Instance * __accumulate(Engine *engine, Node *vargs, Frame *frame):
    cdef:
        List *args = __resolve(engine, vargs, frame)
        Node *arg_cursor = args.head
        Instance *inst_a
        Instance *inst_b
        int int_a, int_b, result

    inst_a = <Instance *> arg_cursor.value
    int_a = (<int *> inst_a.data)[0]

    arg_cursor = arg_cursor.next

    inst_b = <Instance *> arg_cursor.value
    int_b = (<int *> inst_b.data)[0]

    result = int_a + int_b
    return instance_wrap_int(result, TYPE_NUMBER)


cdef Instance * __print_function(Engine *engine, Node *vargs, Frame *frame):
    cdef:
        List *args = __resolve(engine, vargs, frame)
        Node *arg_cursor = args.head
        Instance *arg_inst

    while arg_cursor != EMPTY_NODE:
        arg_inst = arg_cursor.value

        if arg_inst.type == TYPE_STRING:
            printf('%s', arg_inst.data)
        elif arg_inst.type == TYPE_NUMBER:
            printf('%i', (<int *> arg_inst.data)[0])

        arg_cursor = arg_cursor.next

    list_free(args)


cdef Instance * __define_function(Engine *engine, Node *args, Frame *frame):
    cdef:
        Node *arg_cursor
        char *function_name
        DynamicFunction *new_func

    arg_cursor = args
    new_func = <DynamicFunction *> malloc(sizeof(DynamicFunction))

    # Unpack args
    new_func.symbol = arg_cursor.value
    function_name = <char *> arg_cursor.value.data

    arg_cursor = arg_cursor.next
    new_func.arg_defs = arg_cursor.value

    arg_cursor= arg_cursor.next
    new_func.code = arg_cursor.value

    scope_add(frame_peek(frame), function_name, instance_wrap_ptr(new_func, TYPE_DYNAMIC_FUNC))
