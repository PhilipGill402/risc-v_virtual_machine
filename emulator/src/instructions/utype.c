#include "instructions/utype.h"
#include "instructions/dispatchers/lui.h"
#include "instructions/dispatchers/auipc.h"
#include "cpu.h"
#include "trap.h"

void executeU(cpu_t* cpu, utype_t instruction) {
    opcode_t opcode = (opcode_t)instruction.opcode;
    
    switch (opcode) {
        case LUI: dispatch_lui(cpu, instruction); break;
        case AUIPC: dispatch_auipc(cpu, instruction); break;
        default: raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, instruction.raw); break;
    }
}
