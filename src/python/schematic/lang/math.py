from schematic.util import resolve
from schematic.lang.types import Function


# [> 0 1] -> False
class GreaterThanOrEqualTo(Function):

    def run(self, engine, parameters, scope):
        a, b = resolve(engine, scope, parameters)
        return a >= b


# [> 0 1] -> False
class GreaterThan(Function):

    def run(self, engine, parameters, scope):
        a, b = resolve(engine, scope, parameters)
        return a > b


# [< 0 1] -> True
class LessThan(Function):

    def run(self, engine, parameters, scope):
        a, b = resolve(engine, scope, parameters)
        return a < b


# [< 0 1] -> True
class LessThanOrEqualTo(Function):

    def run(self, engine, parameters, scope):
        a, b = resolve(engine, scope, parameters)
        return a <= b


# [= 0 0] -> True
class Equals(Function):

    def run(self, engine, parameters, scope):
        a, b = resolve(engine, scope, parameters)
        return a == b


# [+ 1 1] -> 2
class Accumulate(Function):

    def run(self, engine, parameters, scope):
        a, b = resolve(engine, scope, parameters)
        return a + b


# [/ 2 1] -> 2
class Quotient(Function):

    def run(self, engine, parameters, scope):
        a, b = resolve(engine, scope, parameters)
        return a / b


# [* 2 1] -> 2
class Product(Function):

    def run(self, engine, parameters, scope):
        a, b = resolve(engine, scope, parameters)
        return a * b


# [- 1 1] -> 0
class Subtract(Function):

    def run(self, engine, parameters, scope):
        a, b = resolve(engine, scope, parameters)
        return a - b


GRAMMAR = {
    '>=': GreaterThanOrEqualTo(),
    '>': GreaterThan(),
    '<=': LessThanOrEqualTo(),
    '<': LessThan(),
    '=': Equals(),
    '*': Product(),
    '/': Quotient(),
    '+': Accumulate(),
    '-': Subtract()
}
