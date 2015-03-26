import copy

from schematic.util import is_callable, is_symbol
from schematic.lang.function import Function


_EMPTY_CONTEXT = list()


class InterperterException(Exception):

    def __init__(self, msg):
        self.msg = msg

    def __str__(self):
        return self.msg


class InterpretedFunction(Function):

    def __init__(self, name, parameters, code):
        self.name = name
        self.parameters = parameters
        self.code = code

    def run(self, engine, param_values, scope):
        func_scope = copy.copy(scope)

        for i in range(len(self.parameters)):
            func_scope[self.parameters[i]] = param_values[i]

        return engine.lcall(
            code=copy.copy(self.code),
            scope=func_scope)

    def __str__(self):
        return 'function {} ({}): {}'.format(
            self.name,
            self.parameters,
            self.code)


class DefinerFunction(Function):

    def run(self, engine, parameters, scope):
        name = parameters.pop(0)

        engine.define(
            name=name,
            param_defs=parameters.pop(0),
            code=parameters.pop(0))


def lookup(scope, symbol):
    if is_symbol(symbol):
        sym_ref = scope.get(symbol)

        if sym_ref is None:
            raise InterperterException(
                msg='Undefined symbol: {}'.format(symbol))

        return sym_ref
    else:
        return symbol


class Engine(object):

    def __init__(self, global_scope):
        self._gs = global_scope
        self._gs['def'] = DefinerFunction()

    def define(self, name, param_defs, code):
        self._gs[name] = InterpretedFunction(name, param_defs, code)

    def lcall(self, code, scope):
        if not isinstance(code, list):
            return self.call(code, _EMPTY_CONTEXT, scope)
        else:
            return self.call(code.pop(0), code, scope)

    def call(self, symbol, vparams, vscope=None):
        ascope = vscope if vscope is not None else self._gs
        aparams = list()

        for vparam in vparams:
            if is_symbol(vparam):
                aparam = ascope.get(vparam)
                aparams.append(vparam if aparam is None else aparam)
            else:
                aparams.append(vparam)

        # Lookup the symbol
        sym_ref = lookup(ascope, symbol)

        if isinstance(sym_ref, Function):
            # print('{}({})'.format(symbol, aparams))
            return sym_ref.run(self, aparams, ascope)
        else:
            return sym_ref


def run(context, scope):
    engine = Engine(scope)
    opstack = list()

    while True:
        if context is None or len(context) == 0:
            if len(opstack) == 0:
                return

            context = opstack.pop()

        else:
            instruction = context.pop(0)

            if isinstance(instruction, list):
                opstack.append(context)
                context = instruction
            else:
                engine.call(instruction, context)
                context = None
