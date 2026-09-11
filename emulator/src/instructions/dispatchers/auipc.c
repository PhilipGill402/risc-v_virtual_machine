#include "instructions/dispatchers/auipc.h"
#include "cpu.h"

static void auipc(cpu_t* cpu, memory_t* mem, utype_t instruction) {
    (void)mem;
    cpu_write_reg(cpu, instruction.rd, cpu->pc + instruction.imm);
}

void dispatch_auipc(cpu_t* cpu, memory_t* mem, utype_t instruction) {
    auipc(cpu, mem, instruction);
}
