from schematic.common cimport *
from schematic.constants cimport *


ctypedef Instance * (*evaluator)(Engine *, Node *, Frame *)
ctypedef Instance * (*dynamic_evaluator)(Engine *, DynamicFunction *, Node *, Frame *)


ctypedef struct Engine:

    Frame *root_frame
    dynamic_evaluator *dyn_invoke


ctypedef struct Frame:

    List *scope_stack


cdef Instance * resolve(Engine *, Node *, Frame *)
cdef List * resolve_all(Engine *, Node *, Frame *)

cdef Engine * engine_new()
cdef Instance * engine_leval(Engine *, List *, Frame *)
cdef Instance * engine_eval(Engine *, Instance *, Node *, Frame *)

cdef Frame * frame_new()
cdef void frame_push(Frame *, Scope *)
cdef Scope * frame_peek(Frame *)
cdef void frame_pop(Frame *)
cdef Instance * frame_search(Frame *frame, char *name)
