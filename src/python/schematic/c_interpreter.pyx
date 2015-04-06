from libc.stdio cimport printf
from libc.string cimport strlen
from libc.stdlib cimport malloc, free

from schematic.common cimport *
from schematic.engine cimport *
from schematic.constants cimport *
from schematic.std_grammar cimport *


cdef Instance * dynamic_invoke(Engine *engine, DynamicFunction *fdef, Node *args, Frame *frame):
    cdef:
        Scope *fun_scope

        Node *arg_cursor
        Node *arg_def_cursor

        char *arg_name
        Instance *arg_value

    fun_scope = scope_new()

    arg_cursor = args
    arg_def_cursor = (<List *> fdef.arg_defs.data).head

    while arg_cursor != EMPTY_NODE:
        arg_name = <char *> arg_def_cursor.value.data
        arg_def_cursor = arg_def_cursor.next

        arg_value = arg_cursor.value
        arg_cursor = arg_cursor.next

        scope_add(fun_scope, arg_name, arg_value)

    frame_push(frame, fun_scope)
    engine_leval(engine, (<List *> fdef.code.data), frame)

    frame_pop(frame)
    scope_free(fun_scope)


cdef void run(List *doc):
    cdef:
        Engine *engine = engine_new()
        Node *cursor = doc.head

    engine.dyn_invoke = <dynamic_evaluator *> &dynamic_invoke
    frame_push(engine.root_frame, std_grammar())

    while cursor != EMPTY_NODE:
        if cursor.value.type == TYPE_LIST:
            engine_leval(engine, <List *> cursor.value.data, engine.root_frame)

        else:
            printf('Unexpected list type: %i\n', cursor.value.type)

        # Look to the next node
        cursor = cursor.next
