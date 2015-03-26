from schematic.lang.function import Function


_CALLABLE_TYPES = [
    Function,
    list
]


def resolve(engine, scope, objects):
    return unpack_parameters((engine.lcall(o, scope) for o in objects))


def is_symbol(obj):
    if isinstance(obj, str):
        first_ord = ord(obj[0])

        # Symbols can not start with '"' or any number.
        return first_ord != 34 and (first_ord < 48 or first_ord > 57)

    return False


def is_callable(obj):
    return obj is not None and type(obj) in _CALLABLE_TYPES


def unpack_parameters(parameters):
    # Symbols are represented as strings, otherwise don't type unpack
    return (unpack_symbol(s) if isinstance(s, str) else s for s in parameters)


def unpack_symbol(symbol):
    if symbol is not None:
        if isinstance(symbol, str):
            first_ord = ord(symbol[0])

            if first_ord >= 48 and first_ord <= 57:
                number = float(symbol)

                if number % 1 != 0:
                    return number
                else:
                    return int(number)

    return symbol
