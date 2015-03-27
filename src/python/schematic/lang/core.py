from schematic.util import resolve
from schematic.lang.function import Function


class TrueFunction(Function):

    def run(self, engine, parameters, scope):
        return True


class FalseFunction(Function):

    def run(self, engine, parameters, scope):
        return False


class DefinerFunction(Function):

    def run(self, engine, parameters, scope):
        name = parameters.pop(0)

        engine.define(
            name=name,
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


class PrintFunction(Function):

    def run(self, engine, parameters, scope):
        content = resolve(engine, scope, parameters)
        print(''.join([str(c) for c in content]))


GRAMMAR = {
    'def': DefinerFunction(),
    'true': TrueFunction(),
    'false': FalseFunction(),
    'print': PrintFunction(),
    'if': ConditionalFunction()
}
