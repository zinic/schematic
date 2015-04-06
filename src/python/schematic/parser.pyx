from libc.stdio cimport FILE, fopen, fclose, fread, printf
from libc.stdlib cimport realloc, malloc, free

from schematic.common cimport *
from schematic.constants cimport *


# State constants
cdef int S_START = 0
cdef int S_LIST_ITEM = 1
cdef int S_LIST_LITERAL = 2
cdef int S_STRING = 3
cdef int S_NUMBER = 4
cdef int S_SYMBOL = 5
cdef int S_NEG_NUM_OR_SYMBOL = 6


# Helper functions
cdef int is_number(int ch_ord):
    return ch_ord >= ORD_NUMBER_0 and ch_ord <= ORD_NUMBER_9


cdef int is_whitespace(int ch_ord):
    return ch_ord == ORD_TAB \
        or ch_ord == ORD_SPACE \
        or ch_ord == ORD_NEWLINE \
        or ch_ord == ORD_CARRIAGE_RETURN


# Entry point
cdef List * parse(char *source_file):
    cdef FILE *file = fopen(source_file, 'r')
    cdef List *value = _parse(file)

    fclose(file)

    return value


cdef List * _list_descend(List *cursor, List *position_stack):
    cdef List *new_list = list_new()
    list_append(cursor, instance_wrap_ptr(new_list, TYPE_LIST))
    list_append(position_stack, instance_wrap_ptr(cursor, TYPE_LIST))
    return new_list


cdef List * _list_ascend(List *position_stack):
    cdef Instance *inst = list_pop(position_stack)
    cdef void *data = inst.data

    instance_free(inst)

    return <List *> data


cdef List * _parse(FILE *src_fd):
    cdef Accumulator *acc = accumulator_new()
    cdef char buff[1]

    cdef List *doc = list_new()
    cdef List *position_stack = list_new()
    cdef List *cursor = doc

    cdef int escaped = False, read, state = S_START, number = 0, number_mag = 1

    cdef char next_ch

    while True:
        read = fread(&buff, 1, 1, src_fd)

        if read == 0:
            # Done reading
            break

        next_ch = buff[0]

        if state == S_START:
            if is_whitespace(next_ch):
                # Skip leading whitespace
                pass

            elif next_ch == ORD_OPEN_BRACKET:
                cursor = _list_descend(cursor, position_stack)
                state = S_LIST_ITEM

            else:
                printf('Bad token at start: "%c"\n', next_ch)

        elif state == S_LIST_ITEM:
            if is_whitespace(next_ch):
                # Skip leading whitespace
                pass

            elif next_ch == ORD_OPEN_BRACKET:
                cursor = _list_descend(cursor, position_stack)

            elif next_ch == ORD_CLOSE_BRACKET:
                cursor = _list_ascend(position_stack)

            elif next_ch == ORD_DOUBLE_QUOTE:
                state = S_STRING

            elif next_ch == ORD_MINUS:
                state = S_NEG_NUM_OR_SYMBOL

            elif is_number(next_ch):
                state = S_NUMBER
                number_mag = 10
                number = next_ch - ORD_NUMBER_0

            else:
                accumulator_append(acc, next_ch)
                state = S_SYMBOL

        elif state is S_NEG_NUM_OR_SYMBOL:
            if is_whitespace(next_ch):
                list_append(cursor, instance_wrap_ptr(STR_MINUS, TYPE_SYMBOL))
                state = S_LIST_ITEM

            elif is_number(next_ch):
                number = -(next_ch - ORD_NUMBER_0)
                number_mag = 10
                state = S_NUMBER

            elif next_ch is ORD_CLOSE_BRACKET:
                list_append(cursor, instance_wrap_ptr(STR_MINUS, TYPE_SYMBOL))
                state = S_LIST_ITEM
                cursor = _list_ascend(position_stack)

            else:
                accumulator_append(acc, ORD_MINUS)
                accumulator_append(acc, next_ch)
                state = S_SYMBOL

        elif state == S_SYMBOL:
            if is_whitespace(next_ch):
                if accumulator_has_bytes(acc):
                    list_append(cursor, instance_wrap_ptr(accumulator_get(acc), TYPE_SYMBOL))

                state = S_LIST_ITEM

            elif next_ch == ORD_OPEN_BRACKET:
                printf('Bad token at symbol: "%c"\n', next_ch)

            elif next_ch == ORD_CLOSE_BRACKET:
                if accumulator_has_bytes(acc):
                    list_append(cursor, instance_wrap_ptr(accumulator_get(acc), TYPE_SYMBOL))

                cursor = _list_ascend(position_stack)
                state = S_LIST_ITEM

            else:
                accumulator_append(acc, next_ch)

        elif state is S_NUMBER:
            if is_whitespace(next_ch):
                list_append(cursor, instance_wrap_int(number, TYPE_NUMBER))
                state = S_LIST_ITEM

            elif next_ch is ORD_CLOSE_BRACKET:
                list_append(cursor, instance_wrap_int(number, TYPE_NUMBER))

                cursor = _list_ascend(position_stack)
                state = S_LIST_ITEM

            else:
                number *= number_mag
                number += next_ch - ORD_NUMBER_0
                number_mag *= 10

        elif state == S_STRING:
            if escaped == True:
                escaped = False

                if next_ch == ORD_N:
                    accumulator_append(acc, ORD_NEWLINE)
                elif next_ch == ORD_R:
                    accumulator_append(acc, ORD_CARRIAGE_RETURN)
                elif next_ch == ORD_T:
                    accumulator_append(acc, ORD_TAB)
                elif next_ch == ORD_DOUBLE_QUOTE:
                    accumulator_append(acc, ORD_DOUBLE_QUOTE)
                elif next_ch == ORD_BACKSLASH:
                    accumulator_append(acc, ORD_BACKSLASH)
                else:
                    printf('Unknown escape sequence "\\%c"', next_ch)
            else:
                if next_ch == ORD_DOUBLE_QUOTE:
                    if accumulator_has_bytes(acc):
                        list_append(cursor, instance_wrap_ptr(accumulator_get(acc), TYPE_STRING))

                    state = S_LIST_ITEM
                elif next_ch == ORD_BACKSLASH:
                    escaped = True

                else:
                    accumulator_append(acc, next_ch)

    accumulator_free(acc)
    return doc
