from schematic.lang.function import Function


class TrueFunction(Function):

    def run(self, engine, parameters, scope):
        return True


class FalseFunction(Function):

    def run(self, engine, parameters, scope):
        return False


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
        content = parameters[0]
        print(''.join(content))


GRAMMAR = {
    'true': TrueFunction(),
    'false': FalseFunction(),
    '.print': PrintFunction(),
    '.if': ConditionalFunction()
}
