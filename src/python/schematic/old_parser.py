from schematic.lang.types import Symbol, List
from schematic.lang.errors import CoreError


S_START = 0
S_LIST_ITEM = 1
S_LIST_LITERAL = 2
S_STRING = 3
S_NUMBER = 4
S_SYMBOL = 5
S_NEG_NUM_OR_SYMBOL = 6

ORD_N = ord('n')
ORD_R = ord('r')
ORD_T = ord('t')

ORD_TAB = ord('\t')
ORD_SPACE = ord(' ')
ORD_NEWLINE = ord('\n')
ORD_CARRIAGE_RETURN = ord('\r')

ORD_MINUS = ord('-')
ORD_NUMBER_0 = ord('0')
ORD_NUMBER_9 = ord('9')

ORD_BACKSLASH = ord('\\')
ORD_DOUBLE_QUOTE = ord('"')
ORD_OPEN_BRACKET = ord('[')
ORD_CLOSE_BRACKET = ord(']')

STR_MINUS = "-"


class BadTokenException(CoreError):
    pass


def is_number(ch_ord):
    return ch_ord >= ORD_NUMBER_0 and ch_ord <= ORD_NUMBER_9


def is_whitespace(ch_ord):
    return (ch_ord == ORD_TAB
        or ch_ord == ORD_SPACE
        or ch_ord == ORD_NEWLINE
        or ch_ord == ORD_CARRIAGE_RETURN)


class ByteAccumulator(object):

    def __init__(self, size=4096):
        self._write_idx = 0
        self._buffer = bytearray(size)

    def append(self, byte):
        self._buffer[self._write_idx] = byte
        self._write_idx += 1

    def has_bytes(self):
        return self._write_idx > 0

    def get(self):
        content_slice = self._buffer[0:self._write_idx]
        self._write_idx = 0
        return content_slice.decode()


def parse(source_file):
    with open(source_file, 'r') as sin:
        return _parse(sin)


def _parse(sin):
    doc = list()
    buff = bytearray(1)
    state = S_START
    cursor = doc
    escaped = False
    number = 0
    number_mag = 1
    accumulator = ByteAccumulator()
    position_stack = list()

    while True:
        bytes_read = sin.readinto(buff)

        if bytes_read == 0:
            # Done reading
            break

        next_ch = buff[0]

        if state is S_START:
            if is_whitespace(next_ch):
                continue

            elif next_ch == ORD_OPEN_BRACKET:
                new_list = List()
                cursor.append(new_list)
                position_stack.append(cursor)
                cursor = new_list

                state = S_LIST_ITEM

            else:
                raise BadTokenException(next_ch)

        elif state is S_LIST_ITEM:
            if is_whitespace(next_ch):
                pass

            elif next_ch == ORD_OPEN_BRACKET:
                new_list = List()
                cursor.append(new_list)
                position_stack.append(cursor)
                cursor = new_list

            elif next_ch == ORD_CLOSE_BRACKET:
                cursor = position_stack.pop()

            elif next_ch == ORD_DOUBLE_QUOTE:
                state = S_STRING

            elif is_number(next_ch):
                number = (next_ch - ORD_NUMBER_0)
                number_mag = 10
                state = S_NUMBER

            elif next_ch == ORD_MINUS:
                state = S_NEG_NUM_OR_SYMBOL

            else:
                accumulator.append(next_ch)
                state = S_SYMBOL

        elif state is S_NEG_NUM_OR_SYMBOL:
            if is_whitespace(next_ch):
                cursor.append(Symbol(STR_MINUS))
                state = S_LIST_ITEM

            elif is_number(next_ch):
                number = -(next_ch - ORD_NUMBER_0)
                number_mag = 10
                state = S_NUMBER

            elif next_ch is ORD_CLOSE_BRACKET:
                cursor.append(Symbol(STR_MINUS))
                state = S_LIST_ITEM
                cursor = position_stack.pop()

            else:
                accumulator.append(ORD_MINUS)
                accumulator.append(next_ch)
                state = S_SYMBOL

        elif state is S_SYMBOL:
            if is_whitespace(next_ch):
                if accumulator.has_bytes():
                    cursor.append(Symbol(accumulator.get()))
                    state = S_LIST_ITEM

            elif next_ch is ORD_CLOSE_BRACKET:
                if accumulator.has_bytes():
                    cursor.append(Symbol(accumulator.get()))
                    state = S_LIST_ITEM

                cursor = position_stack.pop()

            else:
                accumulator.append(next_ch)

        elif state is S_NUMBER:
            if is_whitespace(next_ch):
                cursor.append(number)
                state = S_LIST_ITEM

            elif next_ch is ORD_CLOSE_BRACKET:
                cursor.append(number)
                state = S_LIST_ITEM
                cursor = position_stack.pop()

            else:
                number *= number_mag
                number += next_ch - ORD_NUMBER_0
                number_mag *= 10

        elif state is S_STRING:
            if escaped:
                    escaped = False

                    if next_ch == ORD_N:
                        accumulator.append(ORD_NEWLINE)
                    elif next_ch == ORD_R:
                        accumulator.append(ORD_CARRIAGE_RETURN)
                    elif next_ch == ORD_T:
                        accumulator.append(ORD_TAB)
                    elif next_ch == ORD_DOUBLE_QUOTE:
                        accumulator.append(ORD_DOUBLE_QUOTE)
                    elif next_ch == ORD_BACKSLASH:
                        accumulator.append(ORD_BACKSLASH)
                    else:
                        raise BadTokenException(
                            'Unknown escape sequence "\\{}"'.format(
                                chr(next_ch)))
            else:
                if next_ch == ORD_DOUBLE_QUOTE:
                    cursor.append(accumulator.get())
                    state = S_LIST_ITEM

                elif next_ch == ORD_BACKSLASH:
                    escaped = True

                else:
                    accumulator.append(next_ch)

    return doc