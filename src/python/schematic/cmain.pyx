from schematic.common cimport *
from schematic.constants cimport *
from schematic.parser cimport parse
from schematic.c_interpreter cimport run

from libc.stdio cimport printf


cpdef void start(char *filename):
    cdef List *doc = parse(filename)

    run(doc)

    #lprint(doc, 0)
    #printf('\n')

    # list_test()


cdef void list_test():
    cdef List *doc = list_new()

    list_append(doc, instance_wrap_ptr("BALLS", 0))
    list_append(doc, instance_wrap_ptr("BALLS", 0))
    list_append(doc, instance_wrap_ptr("BALLS", 0))
    lprint(doc, 0)

    list_pop(doc)
    lprint(doc, 0)

    list_pop(doc)
    lprint(doc, 0)

    list_pop(doc)
    lprint(doc, 0)


cdef void lprint(List *list, int depth):
    cdef Node *current = list.head
    cdef int i

    for i in range(depth):
        printf('  ')

    printf('[')

    if not list_empty(list):
        while True:
            if current.value.type == TYPE_LIST:
                printf('\n')
                lprint(<List *> current.value.data, depth + 1)

            elif current.value.type == TYPE_SYMBOL:
                printf('sym:%s ', <char *> current.value.data)

            elif current.value.type == TYPE_STRING:
                printf('"%s" ', <char *> current.value.data)

            elif current.value.type == TYPE_NUMBER:
                printf('num: %i" ', (<int *> current.value.data)[0])

            if current.next == EMPTY_NODE:
                break

            current = current.next

    printf(']')
