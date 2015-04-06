import copy

from schematic.util import resolve

from schematic.lang.errors import CoreError
from schematic.lang.types import Symbol, Function


_EMPTY_CONTEXT = tuple()


class InterperterException(CoreError):
    pass


class InterpretedFunction(Function):

    def __init__(self, name, parameters, code):
        self.name = name
        self.parameters = parameters
        self.code = code

    def run(self, engine, vparams, scope):
        func_scope = copy.copy(scope)

        i = 0
        for param in resolve(engine, func_scope, vparams):
            pname = self.parameters[i].name
            func_scope[pname] = param
            i += 1

        return engine.lcall(
            code=self.code,
            vscope=func_scope)

    def __str__(self):
        return 'function {} ({}): {}'.format(
            self.name,
            self.parameters,
            self.code)


def lookup(scope, symbol):
    sym_ref = scope.get(symbol)

    if sym_ref is None:
        raise InterperterException(
            msg='Undefined symbol: {}'.format(symbol))

    return sym_ref


class Engine(object):

    def __init__(self, global_scope):
        self._gs = global_scope

    def define(self, name, param_defs, code):
        self._gs[name] = InterpretedFunction(name, param_defs, code)

    def lcall(self, code, vscope=None, a=None):
        scope = vscope if vscope is not None else self._gs

        if isinstance(code, list):
            return self.call(code[0], code[1:], scope)

        return self.call(code, _EMPTY_CONTEXT, scope)

    def call(self, instruction, params, vscope=None):
        scope = vscope if vscope is not None else self._gs

        # Lookup the symbol
        if isinstance(instruction, Symbol):
            sym_ref = lookup(scope, instruction.name)

            if isinstance(sym_ref, Function):
                #print('call {}({})'.format(instruction, params))
                return sym_ref.run(self, params, scope)
            else:
                #print('symbol {} -> {}'.format(instruction, sym_ref))
                return sym_ref
        else:
            #print('statement {}'.format(instruction))
            return instruction


def run(code, scope):
    engine = Engine(scope)

    for statement in code:
        engine.lcall(statement, scope)
