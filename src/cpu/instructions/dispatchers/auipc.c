#include "cpu/instructions/dispatchers/auipc.h"
#include "cpu/cpu.h"

#include <stdio.h>

static void auipc(cpu_t* cpu, utype_t instruction) {
    cpu_write_reg(cpu, instruction.rd, cpu->pc + instruction.imm);
}

void dispatch_auipc(cpu_t* cpu, utype_t instruction) {
    auipc(cpu, instruction);
}
