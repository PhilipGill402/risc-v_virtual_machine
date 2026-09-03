#include "instructions/jtype.h"
#include "cpu.h"
#include <stdio.h>

void jal(CPU& cpu, Memory& mem, JType instruction) {
    int64_t offset = static_cast<int64_t>(instruction.imm);
    uint64_t old_pc = cpu.pc;
    uint64_t j_addr = old_pc + offset;

    cpu.pc = j_addr;

    cpu.write_reg(instruction.rd, old_pc + 4);
}

void executeJ(CPU& cpu, Memory& mem, JType instruction) {
    if (static_cast<Opcode>(instruction.opcode) != Opcode::JAL) {
        fprintf(stderr, "Illegal instruction\n");
        return;
    }

    jal(cpu, mem, instruction);
}
