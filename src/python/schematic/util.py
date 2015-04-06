_EMPTY = tuple()


def resolve(engine, scope, terms=_EMPTY):
    return (engine.lcall(t, scope) for t in terms)
