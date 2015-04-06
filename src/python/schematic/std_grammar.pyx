from libc.stdio cimport printf
from libc.stdlib cimport malloc, free

from schematic.common cimport *
from schematic.engine cimport *
from schematic.constants cimport *


cdef Scope * std_grammar():
    cdef Scope *grammar = scope_new()

    scope_add(grammar, '*', instance_wrap_ptr(
        &__product, TYPE_NATIVE_FUNC))

    scope_add(grammar, '+', instance_wrap_ptr(
        &__accumulate, TYPE_NATIVE_FUNC))

    scope_add(grammar, '-', instance_wrap_ptr(
        &__subtract, TYPE_NATIVE_FUNC))

    scope_add(grammar, '/', instance_wrap_ptr(
        &__quotient, TYPE_NATIVE_FUNC))

    scope_add(grammar, '=', instance_wrap_ptr(
        &__equal, TYPE_NATIVE_FUNC))

    scope_add(grammar, '<', instance_wrap_ptr(
        &__less_than, TYPE_NATIVE_FUNC))

    scope_add(grammar, '<=', instance_wrap_ptr(
        &__less_than_or_equal, TYPE_NATIVE_FUNC))

    scope_add(grammar, '>', instance_wrap_ptr(
        &__greater_than, TYPE_NATIVE_FUNC))

    scope_add(grammar, '>=', instance_wrap_ptr(
        &__greater_than_or_equal, TYPE_NATIVE_FUNC))

    scope_add(grammar, 'if', instance_wrap_ptr(
        &__conditional, TYPE_NATIVE_FUNC))

    scope_add(grammar, 'def', instance_wrap_ptr(
        &__define_function, TYPE_NATIVE_FUNC))

    scope_add(grammar, 'print', instance_wrap_ptr(
        &__print_function, TYPE_NATIVE_FUNC))

    return grammar
    

cdef Instance * __equal(Engine *engine, Node *vargs, Frame *frame):
    cdef:
        List *args = resolve_all(engine, vargs, frame)
        Node *arg_cursor = args.head
        Instance *inst_a
        Instance *inst_b
        int int_a, int_b, result = False

    inst_a = arg_cursor.value
    arg_cursor = arg_cursor.next
    inst_b = arg_cursor.value

    if inst_a.type == TYPE_NUMBER and inst_b.type == TYPE_NUMBER:
        int_a = ptr_void_to_int(inst_a.data)
        int_b = ptr_void_to_int(inst_b.data)

        result = (int_a == int_b)

    return instance_wrap_int(result)


cdef Instance * __less_than(Engine *engine, Node *vargs, Frame *frame):
    cdef:
        List *args = resolve_all(engine, vargs, frame)
        Node *arg_cursor = args.head
        Instance *inst_a
        Instance *inst_b
        int int_a, int_b, result = False

    inst_a = arg_cursor.value
    arg_cursor = arg_cursor.next
    inst_b = arg_cursor.value

    if inst_a.type == TYPE_NUMBER and inst_b.type == TYPE_NUMBER:
        int_a = ptr_void_to_int(inst_a.data)
        int_b = ptr_void_to_int(inst_b.data)

        result = (int_a < int_b)

    return instance_wrap_int(result)


cdef Instance * __greater_than(Engine *engine, Node *vargs, Frame *frame):
    cdef:
        List *args = resolve_all(engine, vargs, frame)
        Node *arg_cursor = args.head
        Instance *inst_a
        Instance *inst_b
        int int_a, int_b, result = False

    inst_a = arg_cursor.value
    arg_cursor = arg_cursor.next
    inst_b = arg_cursor.value

    if inst_a.type == TYPE_NUMBER and inst_b.type == TYPE_NUMBER:
        int_a = ptr_void_to_int(inst_a.data)
        int_b = ptr_void_to_int(inst_b.data)

        result = (int_a > int_b)

    return instance_wrap_int(result)


cdef Instance * __less_than_or_equal(Engine *engine, Node *vargs, Frame *frame):
    cdef:
        List *args = resolve_all(engine, vargs, frame)
        Node *arg_cursor = args.head
        Instance *inst_a
        Instance *inst_b
        int int_a, int_b, result = False

    inst_a = arg_cursor.value
    arg_cursor = arg_cursor.next
    inst_b = arg_cursor.value

    if inst_a.type == TYPE_NUMBER and inst_b.type == TYPE_NUMBER:
        int_a = ptr_void_to_int(inst_a.data)
        int_b = ptr_void_to_int(inst_b.data)

        result = (int_a <= int_b)

    return instance_wrap_int(result)


cdef Instance * __greater_than_or_equal(Engine *engine, Node *vargs, Frame *frame):
    cdef:
        List *args = resolve_all(engine, vargs, frame)
        Node *arg_cursor = args.head
        Instance *inst_a
        Instance *inst_b
        int int_a, int_b, result = False

    inst_a = arg_cursor.value
    arg_cursor = arg_cursor.next
    inst_b = arg_cursor.value

    if inst_a.type == TYPE_NUMBER and inst_b.type == TYPE_NUMBER:
        int_a = ptr_void_to_int(inst_a.data)
        int_b = ptr_void_to_int(inst_b.data)

        result = (int_a >= int_b)

    return instance_wrap_int(result)


cdef Instance * __conditional(Engine *engine, Node *vargs, Frame *frame):
    cdef:
        Instance *conditional_result = resolve(engine, vargs, frame)
        Node *arg_cursor = vargs.next

    if (<int *> conditional_result.data)[0] == True:
        return resolve(engine, arg_cursor, frame)
    elif arg_cursor.next != NULL:
        return resolve(engine, arg_cursor.next, frame)

    return NULL


cdef Instance * __accumulate(Engine *engine, Node *vargs, Frame *frame):
    cdef:
        List *args = resolve_all(engine, vargs, frame)
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
    return instance_wrap_int(result)


cdef Instance * __product(Engine *engine, Node *vargs, Frame *frame):
    cdef:
        List *args = resolve_all(engine, vargs, frame)
        Node *arg_cursor = args.head
        Instance *inst_a
        Instance *inst_b
        int int_a, int_b, result

    inst_a = <Instance *> arg_cursor.value
    int_a = (<int *> inst_a.data)[0]

    arg_cursor = arg_cursor.next

    inst_b = <Instance *> arg_cursor.value
    int_b = (<int *> inst_b.data)[0]

    result = int_a * int_b
    return instance_wrap_int(result)


cdef Instance * __subtract(Engine *engine, Node *vargs, Frame *frame):
    cdef:
        List *args = resolve_all(engine, vargs, frame)
        Node *arg_cursor = args.head
        Instance *inst_a
        Instance *inst_b
        int int_a, int_b, result

    inst_a = <Instance *> arg_cursor.value
    int_a = (<int *> inst_a.data)[0]

    arg_cursor = arg_cursor.next

    inst_b = <Instance *> arg_cursor.value
    int_b = (<int *> inst_b.data)[0]

    result = int_a - int_b
    return instance_wrap_int(result)


cdef Instance * __quotient(Engine *engine, Node *vargs, Frame *frame):
    cdef:
        List *args = resolve_all(engine, vargs, frame)
        Node *arg_cursor = args.head
        Instance *inst_a
        Instance *inst_b
        int int_a, int_b, result

    inst_a = <Instance *> arg_cursor.value
    int_a = (<int *> inst_a.data)[0]

    arg_cursor = arg_cursor.next

    inst_b = <Instance *> arg_cursor.value
    int_b = (<int *> inst_b.data)[0]

    result = int_a / int_b
    return instance_wrap_int(result)


cdef Instance * __print_function(Engine *engine, Node *vargs, Frame *frame):
    cdef:
        List *args = resolve_all(engine, vargs, frame)
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
