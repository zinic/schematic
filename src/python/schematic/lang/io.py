import os
import copy

from schematic.util import resolve
from schematic.lang.types import Function


class WriteFunction(Function):

    def run(self, engine, parameters, scope):
        content, file_descriptor = resolve(engine, scope, parameters)

        if not isinstance(content, str) or not isinstance(content, bytearray):
            content = str(content)

        try:
            file_descriptor.write(content)
            file_descriptor.flush()
        except Exception as ex:
            return False

        return True


class ReadFunction(Function):

    def run(self, engine, parameters, scope):
        file_descriptor, = resolve(engine, scope, parameters)
        return file_descriptor.read()


class OpenFunction(Function):

    def run(self, engine, parameters, scope):
        path, mode = resolve(engine, scope, parameters)
        return open(os.path.expanduser(path), mode)


class PrintFunction(Function):

    def run(self, engine, parameters, scope):
        print(''.join([str(c) for c in resolve(engine, scope, parameters)]))


GRAMMAR = {
    'open': OpenFunction(),
    'write': WriteFunction(),
    'print': PrintFunction(),
    'read': ReadFunction()
}
