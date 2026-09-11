#include "instructions/utype.h"
#include "instructions/dispatchers/lui.h"
#include "instructions/dispatchers/auipc.h"
#include "cpu.h"
#include "exception.h"

void executeU(cpu_t* cpu, memory_t* mem, utype_t instruction) {
    opcode_t opcode = (opcode_t)instruction.opcode;
    
    switch (opcode) {
        case LUI: dispatch_lui(cpu, mem, instruction); break;
        case AUIPC: dispatch_auipc(cpu, mem, instruction); break;
        default: raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0); break;
    }
}
