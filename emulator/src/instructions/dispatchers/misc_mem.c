#include "instructions/dispatchers/misc_mem.h"
#include "trap.h"
#include "log.h"

static void fence_i(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    if (instruction.rd != 0 || instruction.rs1 != 0 || instruction.imm != 0) {
        raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, instruction.raw);
        return;
    }

    // no-op for now 
    return;
}

static void fence(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    // no-op for now 
    return;
}

void dispatch_misc_mem(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    switch (instruction.funct3) {
        case 0x0: fence(cpu, mem, instruction); break; 
        case 0x1: fence_i(cpu, mem, instruction); break;
        default: raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, instruction.raw);
    }
}
