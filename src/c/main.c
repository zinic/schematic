#include "vm.h"


int main(int argc, char* argv[]) {
    byte program[] = {
        OPCODE_START,
        OPCODE_LOAD_SHORT,
        REGISTER_A,
        0x01,
        0x00,
        OPCODE_LOAD_SHORT,
        REGISTER_B,
        0xFF,
        0xFF,
        OPCODE_ADD,
        OPCODE_JUMP_IF_ZERO,
        0x01,
        0x00,
        0x00,
        0x00,
        OPCODE_STOP
    };

#ifdef TEST
    #include "test.h"
    run_tests();
    return 0;
#else
    return machine_start(program, sizeof(program));
#endif
}