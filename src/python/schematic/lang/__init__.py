from .core import GRAMMAR as CORE
from .io import GRAMMAR as IO
from .math import GRAMMAR as MATH
from .types import GRAMMAR as TYPES

GRAMMARS = [
    CORE,
    IO,
    MATH,
    TYPES
]


def std_grammar():
    grammar = dict()

    for ext_grammar in GRAMMARS:
        for name, definition in ext_grammar.iteritems():
            grammar[name] = definition

    return grammar
