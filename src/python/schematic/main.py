import sys

from schematic.lang import std_grammar
from schematic.parser import parse
from schematic.interpreter import run


def main(script):
    run(parse(script), std_grammar())


main(sys.argv[1])
