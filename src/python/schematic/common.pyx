from libc.stdio cimport printf
from libc.string cimport strlen
from libc.stdlib cimport malloc, free

# Constant declarations
TYPE_SYMBOL = 0
TYPE_STRING = 1
TYPE_NUMBER = 2
TYPE_LIST = 3
TYPE_NATIVE_FUNC = 4
TYPE_DYNAMIC_FUNC = 5

EMPTY_INSTANCE = <Instance *> NULL
EMPTY_NODE = <Node *> NULL


cdef char * type_to_str(Type t):
    if t == TYPE_SYMBOL:
        return 'Symbol'

    elif t == TYPE_STRING:
        return 'String'

    elif t == TYPE_NUMBER:
        return 'Number'

    elif t == TYPE_LIST:
        return 'List'

    elif t == TYPE_NATIVE_FUNC:
        return 'native_function'

    elif t == TYPE_DYNAMIC_FUNC:
        return 'dynamic_function'


# Simple functions
cdef char * cstr(size_t size):
    cdef char *new_str = <char *> malloc(sizeof(char) * size)
    new_str[size] = 0
    return new_str


cdef int str_int_val(char *str, size_t length):
    cdef int i, val = 0

    for i in range(length):
        val += str[i]

    return val


cdef int ptr_void_to_int(void *ptr):
    cdef int *iptr = <int *> ptr
    return iptr[0]


# Instance functions
cdef void instance_print(Instance *instance):
    cdef Type instance_type

    if instance == NULL:
        printf('null')
    else:
        instance_type = instance.type

        printf('type:%s -> ', type_to_str(instance_type))

        if instance_type == TYPE_SYMBOL:
            printf('%s', instance.data)

        elif instance_type == TYPE_STRING:
            printf('"%s"', instance.data)

        elif instance_type == TYPE_NUMBER:
            printf('%i', (<int *> instance.data)[0])

        elif instance_type == TYPE_LIST:
            printf('size: %i', (<List *> instance.data).length)

        elif instance_type == TYPE_NATIVE_FUNC:
            printf('%u', instance.data)

        elif instance_type == TYPE_DYNAMIC_FUNC:
            printf('%s', (<DynamicFunction *> instance.data).symbol.data)

        else:
            printf('unknown_type(%i)', instance_type)


cdef Instance * instance_wrap_ptr(void *value, Type type):
    cdef Instance *new_instance = <Instance *> malloc(sizeof(Instance))

    new_instance.data = value
    new_instance.type = type

    return new_instance


cdef Instance * instance_wrap_int(int value):
    cdef int *int_ptr = <int *> malloc(sizeof(int))
    int_ptr[0] = value

    cdef Instance *new_instance = <Instance *> malloc(sizeof(Instance))

    new_instance.data = <void *> int_ptr
    new_instance.type = TYPE_NUMBER

    return new_instance


cdef void instance_free(Instance *instance):
    free(instance)


cdef Scope * scope_new():
    cdef Scope *scope = <Scope *> malloc(sizeof(Scope))
    scope.instances = NULL

    return scope


cdef void scope_add(Scope *scope, char *name, Instance *inst):
    cdef ScopedInstance *scoped_inst = <ScopedInstance *> malloc(sizeof(ScopedInstance))

    scoped_inst.instance = inst
    scoped_inst.name = name
    scoped_inst.h_id = str_int_val(name, strlen(name))

    cdef extern:
        cdef C_VARNAME __VARNAME_ID "h_id"

    HASH_ADD_INT(scope.instances, __VARNAME_ID, scoped_inst)


cdef ScopedInstance * scope_get(Scope *scope, char *name):
    cdef:
        ScopedInstance *scoped_inst = NULL
        int h_id = str_int_val(name, strlen(name))

    HASH_FIND_INT(scope.instances, &h_id, scoped_inst)

    return scoped_inst


cdef void scope_free(Scope *scope):
    pass


# Node functions
cdef Node * node_new(Instance *value):
    cdef Node *new_node = <Node *> malloc(sizeof(Node))

    new_node.value = value
    new_node.next = EMPTY_NODE
    new_node.previous = EMPTY_NODE

    return new_node


cdef void node_free(Node *node):
    free(node)


# List functions
cdef List * list_new():
    cdef List *new_list = <List *> malloc(sizeof(List))

    new_list.length = 0
    new_list.head = EMPTY_NODE
    new_list.tail = EMPTY_NODE

    return new_list


cdef List * list_free(List *list):
    cdef:
        Node *ref
        Node *cursor

    cursor = list.head
    while cursor != EMPTY_NODE:
        ref = cursor.next
        free(cursor)
        cursor = ref

    free(list)


cdef int list_empty(List *list):
    return list.length == 0


cdef void list_append(List *list, Instance *instance):
    cdef Node *new_node = node_new(instance)
    cdef Node *previous_tail

    list.length += 1

    # Is this an empty list?
    if list.head == EMPTY_NODE:
        # Set as the head
        list.head = new_node

    elif list.tail == EMPTY_NODE:
        # Set as the tail
        list.tail = new_node

        # Link the head node to the next node
        _link_nodes(list.head, new_node)

    else:
        # Link the old tail to the next node
        previous_tail = list.tail
        _link_nodes(previous_tail, new_node)

        # Set as the tail
        list.tail = new_node


cdef Node * list_tail(List *list):
    cdef Node *node = EMPTY_NODE

    if list.head != EMPTY_NODE:
        # Do we have any items?

        if list.tail != EMPTY_NODE:
            # Do we have more than one item?

            if list.head == list.tail.previous:
                # Are there only two items left?
                node = list.tail
            else:
                # There are more than two items in the list
                node = list.tail

        else:
            # Pop the head since it's the last element
            node = list.head

    return node


cdef Instance * list_peek(List *list):
    cdef const Node * node = list_tail(list)
    cdef Instance * node_value = NULL

    if node != EMPTY_NODE:
        # Unpack the instance
        node_value = node.value

    return node_value


cdef Instance * list_pop(List *list):
    cdef Instance *node_value = EMPTY_INSTANCE
    cdef Node *node = EMPTY_NODE

    list.length -= 1

    if list.head != EMPTY_NODE:
        # Do we have any items?

        if list.tail != EMPTY_NODE:
            # Do we have more than one item?

            if list.head == list.tail.previous:
                # Are there only two items left?
                node = list.tail
                list.tail = EMPTY_NODE

                _unlink_node(node)
                _unlink_node(list.head)
            else:
                # There are more than two items in the list
                node = list.tail

                list.tail = node.previous
                list.tail.next = EMPTY_NODE

                _unlink_node(node)

        else:
            # Pop the head since it's the last element
            node = list.head

            list.head = EMPTY_NODE
            _unlink_node(node)


    if node != EMPTY_NODE:
        # Unpack the instance and get rid of the node
        node_value = node.value
        node_free(node)

    return node_value


cdef void _link_nodes(Node *first, Node *last):
    first.next = last
    last.previous = first


cdef void _unlink_node(Node *node):
    node.next = EMPTY_NODE
    node.previous = EMPTY_NODE


# Accumulator functions
cdef Accumulator * accumulator_new():
    cdef Accumulator *acc = <Accumulator *> malloc(sizeof(Accumulator))
    acc.buffer = cstr(4096)
    acc.write_idx = 0

    return acc


cdef int accumulator_has_bytes(Accumulator *acc):
    return acc.write_idx > 0


cdef void accumulator_free(Accumulator *acc):
    free(acc.buffer)


cdef void accumulator_append(Accumulator *acc, char byte):
    acc.buffer[acc.write_idx] = byte
    acc.write_idx += 1


cdef char * accumulator_get(Accumulator *acc):
    cdef char *container = cstr(acc.write_idx)
    cdef int i

    for i in range(acc.write_idx):
        container[i] = acc.buffer[i]

    acc.write_idx = 0
    return container
