import sys

from schematic.lang import std_grammar
from schematic.lang.errors import CoreError
from schematic.parser import parse
from schematic.interpreter import run


def main(script):
    try:
        run(parse(script), std_grammar())
    except CoreError as ce:
        print(ce)

main(sys.argv[1])
