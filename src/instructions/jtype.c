#include "instructions/jtype.h"
#include "cpu.h"
#include <stdio.h>

void jal(cpu_t* cpu, memory_t* mem, jtype_t instruction) {
    int64_t offset = (int64_t)instruction.imm;
    uint64_t old_pc = cpu->pc;
    uint64_t j_addr = old_pc + offset;

    cpu->pc = j_addr;

    cpu_write_reg(cpu, instruction.rd, old_pc + 4);
}

void executeJ(cpu_t* cpu, memory_t* mem, jtype_t instruction) {
    if ((opcode_t)instruction.opcode != JAL) {
        fprintf(stderr, "Illegal instruction\n");
        return;
    }

    jal(cpu, mem, instruction);
}
