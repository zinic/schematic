#ifndef vm_h
#define vm_h

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
    
    
// Return codes
#define ERROR_INVALID_OPCODE 1
#define ERROR_INVALID_REGISTER 2

// Type information
#define BYTE_SIZE sizeof(byte)
#define SHORT_SIZE sizeof(short)
#define INT_SIZE sizeof(int)
#define LONG_SIZE sizeof(long)
#define FLOAT_SIZE sizeof(float)
#define DOUBLE_SIZE sizeof(double)
#define REGISTER_SIZE sizeof(long)
#define UINT_SIZE sizeof(unsigned int)

// Operation codes!
#define OPCODE_START 0x00
#define OPCODE_JUMP 0x02
#define OPCODE_JUMP_IF_ZERO 0x03
#define OPCODE_STOP 0x0F

#define OPCODE_LOAD_BYTE 0x10
#define OPCODE_LOAD_SHORT 0x11
#define OPCODE_LOAD_INT 0x12
#define OPCODE_LOAD_LONG 0x13
#define OPCODE_LOAD_FLOAT 0x14
#define OPCODE_LOAD_DOUBLE 0x15

#define OPCODE_ADD 0x20
#define OPCODE_SUBTRACT 0x21
#define OPCODE_PRODUCT 0x22
#define OPCODE_QUOTIENT 0x23

#define OPCODE_FLOAT_ADD 0x30
#define OPCODE_FLOAT_SUBTRACT 0x31
#define OPCODE_FLOAT_PRODUCT 0x32
#define OPCODE_FLOAT_QUOTIENT 0x33

#define OPCODE_DOUBLE_ADD 0x43
#define OPCODE_DOUBLE_SUBTRACT 0x44
#define OPCODE_DOUBLE_PRODUCT 0x45
#define OPCODE_DOUBLE_QUOTIENT 0x46

// Register definitions
#define REGISTER_A 0x00
#define REGISTER_B 0x01
#define REGISTER_R 0x02

// Timing junk
#define SEC_IN_NANOS 1000000000
#define PERF_ITERATIONS 100000000

#define panic(retcode, reason)\
    printf(reason);

// Easy to use macros
#define pc_advance(proc, amount) proc->pc += amount
#define coerce(symbol, type) *((type *) symbol)

#define register_load(program, tcpu, reg_type, data_type)\
switch (next_byte(&program, tcpu)) {\
    case REGISTER_A:\
        coerce(tcpu->ra, reg_type) = coerce(&program.data[tcpu->pc], data_type);\
        pc_advance(tcpu, sizeof(data_type));\
        break;\
\
    case REGISTER_B:\
        coerce(tcpu->rb, reg_type) = coerce(&program.data[tcpu->pc], data_type);\
        pc_advance(tcpu, sizeof(data_type));\
        break;\
}

#define register_load_long(program, tcpu, data_type) register_load(program, tcpu, long, data_type)

#define register_load_double(program, tcpu, data_type) register_load(program, tcpu, double, data_type)
    
    
// Nice name assignments for me
typedef void Register;
typedef unsigned char byte;


// Defines a program
typedef struct Program Program;

struct Program {
    const byte *data;
    const unsigned int size;
};


// We're using a simple four register CPU
typedef struct Processor Processor;

struct Processor {
    unsigned int pc;
    
    Register *ra;
    Register *rb;
    Register *rr;    
};

// Helps us keep track of the processor
typedef struct ProcessorState ProcessorState;

struct ProcessorState {
    Processor *processor;
};

int machine_start(const byte *code, const size_t program_size);
    
#ifdef __cplusplus
}
#endif
#endif