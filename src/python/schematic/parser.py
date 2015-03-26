S_START = 0
S_LIST_ITEM = 1
S_LIST_LITERAL = 2
S_STRING = 3
S_SYMBOL = 4

LIST_START_CH = ord('[')
LIST_END_CH = ord(']')

LIST_LITERAL_ESC_CH = ord('\'')

STRING_CH = ord('"')
WHITESPACE = [ord(' '), ord('\n'), ord('\r'), ord('\t')]

LITERAL_LIST = True


class BadTokenException(Exception):

    def __init__(self, msg):
        self._msg = msg


def parse(source_file):
    with open(source_file, 'r') as sin:
        return _parse(sin)


class List(list):

    def __init__(self, literal=False):
        super(List, self).__init__()
        self._literal = literal

    def is_literal(self):
        return self._literal is LITERAL_LIST


def _parse(sin):
    doc = list()
    buff = bytearray(1)
    state = S_START
    cursor = doc
    symbol = list()
    position_stack = list()

    while True:
        bytes_read = sin.readinto(buff)

        if bytes_read == 0:
            # Done reading
            break

        next_ch = buff[0]

        if state is S_START:
            if next_ch in WHITESPACE:
                continue

            if next_ch == LIST_LITERAL_ESC_CH:
                state = S_LIST_LITERAL

            elif next_ch == LIST_START_CH:
                new_list = List()
                cursor.append(new_list)
                position_stack.append(cursor)
                cursor = new_list

                state = S_LIST_ITEM

            else:
                raise BadTokenException(next_ch)

        elif state is S_LIST_LITERAL:
            if next_ch == LIST_START_CH:
                new_list = List(LITERAL_LIST)
                cursor.append(new_list)
                position_stack.append(cursor)
                cursor = new_list

                state = S_LIST_ITEM
            else:
                raise BadTokenException(next_ch)

        elif state is S_LIST_ITEM:
            if next_ch in WHITESPACE:
                pass

            elif next_ch == LIST_LITERAL_ESC_CH:
                state = S_LIST_LITERAL

            elif next_ch == LIST_START_CH:
                new_list = List()
                cursor.append(new_list)
                position_stack.append(cursor)
                cursor = new_list

            elif next_ch == LIST_END_CH:
                cursor = position_stack.pop()

            elif next_ch == STRING_CH:
                symbol.append(chr(STRING_CH))
                state = S_STRING

            else:
                symbol.append(chr(next_ch))
                state = S_SYMBOL

        elif state is S_SYMBOL:
            if next_ch in WHITESPACE:
                if len(symbol) > 0:
                    cursor.append(''.join(symbol))
                    symbol = list()
                    state = S_LIST_ITEM
            elif next_ch is LIST_END_CH:
                if len(symbol) > 0:
                    cursor.append(''.join(symbol))
                    symbol = list()
                    state = S_LIST_ITEM
                    cursor = position_stack.pop()
            else:
                symbol.append(chr(next_ch))

        elif state is S_STRING:
            if next_ch == STRING_CH:
                symbol.append(chr(STRING_CH))
                cursor.append(''.join(symbol))
                symbol = list()
                state = S_LIST_ITEM
            else:
                symbol.append(chr(next_ch))

    return doc
