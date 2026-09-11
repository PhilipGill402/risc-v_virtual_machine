#include "instructions/dispatchers/jalr.h"
#include "cpu.h"

static void jalr(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t addr = instruction.imm + cpu_read_reg(cpu, instruction.rs1);
    addr &= 0xFFFFFFFFFFFFFFFE; // clear lsb
    cpu_write_reg(cpu, instruction.rd, cpu->pc + 4);
    cpu->pc = addr;
}

void dispatch_jalr(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    jalr(cpu, mem, instruction);
}
