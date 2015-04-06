from libc.stdio cimport printf
from libc.string cimport strlen
from libc.stdlib cimport malloc, free

from schematic.common cimport *
from schematic.engine cimport *
from schematic.constants cimport *
from schematic.std_grammar cimport *


cdef void run(List *doc):
    cdef:
        Engine *engine = engine_new()
        Node *cursor = doc.head

    frame_push(engine.root_frame, std_grammar())

    while cursor != EMPTY_NODE:
        if cursor.value.type == TYPE_LIST:
            engine_leval(engine, <List *> cursor.value.data, engine.root_frame)

        else:
            printf('Unexpected list type: %i\n', cursor.value.type)

        # Look to the next node
        cursor = cursor.next
