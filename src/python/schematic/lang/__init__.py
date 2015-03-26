from .core import GRAMMAR as CORE
from .math import GRAMMAR as MATH

GRAMMARS = [
    CORE,
    MATH
]

def std_grammar():
    grammar = dict()

    for ext_grammar in GRAMMARS:
        for name, definition in ext_grammar.iteritems():
            grammar[name] = definition

    return grammar
