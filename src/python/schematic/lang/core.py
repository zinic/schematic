import os
import copy

from schematic.util import resolve

from schematic.lang.errors import RuntimeError
from schematic.lang.types import Function, Symbol


class WithFunction(Function):

    def run(self, engine, parameters, scope):
        with_scope = copy.copy(scope)

        for scope_dec in parameters.pop(0):
            symbol, statement = scope_dec
            value = engine.lcall(statement, scope)
            with_scope[symbol.name] = value

        last_value = None
        while len(parameters) > 0:
            statement = parameters.pop(0)
            last_value = engine.lcall(statement, with_scope)

        return last_value


class NotFunction(Function):

    def run(self, engine, parameters, scope):
        value, = resolve(engine, scope, parameters)
        return not value


class TrueFunction(Function):

    def run(self, engine, parameters, scope):
        return True


class FalseFunction(Function):

    def run(self, engine, parameters, scope):
        return False


class DefinerFunction(Function):

    def run(self, engine, parameters, scope):
        symbol = parameters.pop(0)

        if not isinstance(symbol, Symbol):
            raise RuntimeError('def function expects a symbol as argument 0')

        engine.define(
            name=symbol.name,
            param_defs=parameters.pop(0),
            code=parameters.pop(0))


class ConditionalFunction(Function):

    def run(self, engine, parameters, scope):
        condition = parameters[0]
        then = parameters[1]
        otherwise = None

        if len(parameters) > 2:
            otherwise = parameters[2]

        condition_result = engine.lcall(condition, scope)

        if condition_result is True:
            return engine.lcall(then, scope)
        else:
            return engine.lcall(otherwise, scope)


GRAMMAR = {
    'with': WithFunction(),
    'not': NotFunction(),
    'true': TrueFunction(),
    'false': FalseFunction(),
    'def': DefinerFunction(),
    'if': ConditionalFunction(),
}
