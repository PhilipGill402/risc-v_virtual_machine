#include "instructions/btype.h"
#include "cpu.h"
#include <stdio.h>

static void beq(CPU& cpu, Memory& mem, BType instruction) {
    uint64_t rs1 = cpu.read_reg(instruction.rs1);
    uint64_t rs2 = cpu.read_reg(instruction.rs2);

    if (rs1 != rs2)
        return;

    cpu.pc = static_cast<int64_t>(instruction.imm);
}

static void bne(CPU& cpu, Memory& mem, BType instruction) {
    uint64_t rs1 = cpu.read_reg(instruction.rs1);
    uint64_t rs2 = cpu.read_reg(instruction.rs2);

    if (rs1 == rs2)
        return;

    cpu.pc += static_cast<int64_t>(instruction.imm);
}

static void blt(CPU& cpu, Memory& mem, BType instruction) {
    uint64_t rs1 = cpu.read_reg(instruction.rs1);
    uint64_t rs2 = cpu.read_reg(instruction.rs2);

    if (rs1 >= rs2)
        return;

    cpu.pc += static_cast<int64_t>(instruction.imm);
}

static void bge(CPU& cpu, Memory& mem, BType instruction) {
    uint64_t rs1 = cpu.read_reg(instruction.rs1);
    uint64_t rs2 = cpu.read_reg(instruction.rs2);

    if (rs1 < rs2)
        return;

    cpu.pc += static_cast<int64_t>(instruction.imm);
}

static void bltu(CPU& cpu, Memory& mem, BType instruction) {
    uint64_t rs1 = cpu.read_reg(instruction.rs1);
    uint64_t rs2 = cpu.read_reg(instruction.rs2);

    if (rs1 >= rs2)
        return;

    cpu.pc += instruction.imm;
}

static void bgeu(CPU& cpu, Memory& mem, BType instruction) {
    uint64_t rs1 = cpu.read_reg(instruction.rs1);
    uint64_t rs2 = cpu.read_reg(instruction.rs2);

    if (rs1 < rs2)
        return;

    cpu.pc += instruction.imm;
}

void executeB(CPU& cpu, Memory& mem, BType instruction) {
    switch(instruction.funct3) {
        case 0x0: beq(cpu, mem, instruction); break;
        case 0x1: bne(cpu, mem, instruction); break;
        case 0x4: blt(cpu, mem, instruction); break;
        case 0x5: bge(cpu, mem, instruction); break;
        case 0x6: bltu(cpu, mem, instruction); break;
        case 0x7: bgeu(cpu, mem, instruction); break;
        default: fprintf(stderr, "Illegal instruction\n");
    }
}
