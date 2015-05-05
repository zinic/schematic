#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm.h"

ProcessorState * ProcessorState_new() {
    // State box for this processor
    ProcessorState *new_procstate = (ProcessorState *) malloc(sizeof(ProcessorState));
    
    // Setup the processor
    Processor *new_proc = (Processor *) malloc(sizeof(Processor));
    memset(new_proc, 0, sizeof(Processor));

    new_proc->ra = malloc(LONG_SIZE);
    new_proc->rb = malloc(LONG_SIZE);
    new_proc->rr = malloc(LONG_SIZE);
    
    new_procstate->processor = new_proc;

    return new_procstate;
}


// Iteration functions
byte next_byte(const Program *program, Processor *proc) {
    return program->data[proc->pc++];
}

int machine_start(const byte *code, const size_t program_size) {
    const Program program = {
        code, program_size
    };
    
    ProcessorState *cpu_state = ProcessorState_new();
    Processor *tcpu = cpu_state->processor;
    
    int retval = 0;
       
    // Timing
    struct timespec last_checkin, now;
    int perf_counter = 0;
    
    clock_gettime(CLOCK_REALTIME, &last_checkin);

    while (tcpu->pc < program_size) {
        const byte next_op = next_byte(&program, tcpu);

#ifdef PC_TRACE
        printf("PC[%d] -> OPCODE %d\n", cpu_state->processor->pc - 1, next_op);
#endif
        
        switch (next_op) {
            case OPCODE_START:
                break;
        
            case OPCODE_STOP:
                goto EXIT;
                
            case OPCODE_LOAD_BYTE:
                register_load_long(program, tcpu, byte);
                break;

            case OPCODE_LOAD_SHORT:
                register_load_long(program, tcpu, short);
                break;

            case OPCODE_LOAD_INT:
                register_load_long(program, tcpu, int);
                break;

            case OPCODE_LOAD_LONG:
                register_load_long(program, tcpu, long);
                break;
                
            case OPCODE_LOAD_DOUBLE:
                register_load_double(program, tcpu, double);
                break;
                
            case OPCODE_JUMP:
                memcpy(&tcpu->pc, &program.data[tcpu->pc], REGISTER_SIZE);
                break;
                
            case OPCODE_JUMP_IF_ZERO:
                if (coerce(tcpu->rr, long) == 0) {
                    memcpy(&tcpu->pc, &program.data[tcpu->pc], REGISTER_SIZE);
                } else {
                    pc_advance(tcpu, INT_SIZE);
                }
                break;

            case OPCODE_ADD:
                memcpy(tcpu->rr, tcpu->ra, REGISTER_SIZE);
                coerce(tcpu->rr, long) += coerce(tcpu->rb, long);
                break;

            case OPCODE_SUBTRACT:
                memcpy(tcpu->rr, tcpu->ra, REGISTER_SIZE);
                coerce(tcpu->rr, long) -= coerce(tcpu->rb, long);
                break;
                
            case OPCODE_PRODUCT:
                memcpy(tcpu->rr, tcpu->ra, REGISTER_SIZE);
                coerce(tcpu->rr, long) *= coerce(tcpu->rb, long);
                break;

            case OPCODE_QUOTIENT:
                memcpy(tcpu->rr, tcpu->ra, REGISTER_SIZE);
                coerce(tcpu->rr, long) /= coerce(tcpu->rb, long);
                break;
                
            case OPCODE_DOUBLE_ADD:
                memcpy(tcpu->rr, tcpu->ra, REGISTER_SIZE);
                coerce(tcpu->rr, double) += coerce(tcpu->rb, double);
                break;
                
            case OPCODE_DOUBLE_SUBTRACT:
                memcpy(tcpu->rr, tcpu->ra, REGISTER_SIZE);
                coerce(tcpu->rr, double) -= coerce(tcpu->rb, double);
                break;

                
            case OPCODE_DOUBLE_PRODUCT:
                memcpy(tcpu->rr, tcpu->ra, REGISTER_SIZE);
                coerce(tcpu->rr, double) *= coerce(tcpu->rb, double);
                break;

            case OPCODE_DOUBLE_QUOTIENT:
                memcpy(tcpu->rr, tcpu->ra, REGISTER_SIZE);
                coerce(tcpu->rr, double) /= coerce(tcpu->rb, double);
                break;
        }

#ifdef PC_TRACE
        printf("PC[%d]\n", cpu_state->processor->pc);
#endif
        
        // Timing
        if (++perf_counter == PERF_ITERATIONS) {
            perf_counter = 0;
            clock_gettime(CLOCK_REALTIME, &now);
            
            long difference_in_seconds = now.tv_sec - last_checkin.tv_sec;
            long difference_in_nanos = now.tv_nsec - last_checkin.tv_nsec;
            
            if (difference_in_nanos < 0) {
                difference_in_seconds--;
                difference_in_nanos += SEC_IN_NANOS;
            }
            
            printf("Average VM loop runtime %f\n", ((double) difference_in_nanos / (double) PERF_ITERATIONS));
                        
            // Copy into the last checkin
            last_checkin = now;
        }
    }
    
    EXIT:    
    return retval;
}