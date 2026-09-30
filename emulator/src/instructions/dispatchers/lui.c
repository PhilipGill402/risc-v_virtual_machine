#include "instructions/dispatchers/lui.h"
#include "cpu.h"

static void lui(cpu_t* cpu, utype_t instruction) {
    cpu_write_reg(cpu, instruction.rd, instruction.imm);
}

void dispatch_lui(cpu_t* cpu, utype_t instruction) {
    lui(cpu, instruction);
}
