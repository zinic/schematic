from schematic.util import resolve


class Integer(object):

    def __init__(self, raw_value):
        self.value = int(raw_value)


class Float(object):

    def __init__(self, raw_value):
        self.value = float(raw_value)


class Function(object):

    def run(self, engine, parameters, namespace):
        raise NotImplementedError()


class Symbol(object):

    def __init__(self, name):
        self.name = name

    def __str__(self):
        return 'sym:{}'.format(self.name)

    def __repr__(self):
        return self.__str__()


class List(list):
    pass


class IntegerFunction(Function):

    def run(self, engine, parameters, scope):
        raw_value, = resolve(engine, scope, parameters)
        return int(raw_value)


class FloatFunction(Function):

    def run(self, engine, parameters, scope):
        raw_value, = resolve(engine, scope, parameters)
        return float(raw_value)


GRAMMAR = {
    'int': IntegerFunction(),
    'float': FloatFunction()
}
