#include "instructions/btype.h"
#include "cpu.h"
#include <stdio.h>

static void beq(cpu_t* cpu, memory_t* mem, btype_t instruction) {
    uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);
    uint64_t rs2 = cpu_read_reg(cpu, instruction.rs2);

    if (rs1 != rs2)
        return;

    cpu->pc += (int64_t)instruction.imm;
}

static void bne(cpu_t* cpu, memory_t* mem, btype_t instruction) {
    uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);
    uint64_t rs2 = cpu_read_reg(cpu, instruction.rs2);

    if (rs1 == rs2)
        return;

    cpu->pc += (int64_t)instruction.imm;
}

static void blt(cpu_t* cpu, memory_t* mem, btype_t instruction) {
    uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);
    uint64_t rs2 = cpu_read_reg(cpu, instruction.rs2);

    if (rs1 >= rs2)
        return;

    cpu->pc += (int64_t)instruction.imm;
}

static void bge(cpu_t* cpu, memory_t* mem, btype_t instruction) {
    uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);
    uint64_t rs2 = cpu_read_reg(cpu, instruction.rs2);

    if (rs1 < rs2)
        return;

    cpu->pc += (int64_t)instruction.imm;
}

static void bltu(cpu_t* cpu, memory_t* mem, btype_t instruction) {
    uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);
    uint64_t rs2 = cpu_read_reg(cpu, instruction.rs2);

    if (rs1 >= rs2)
        return;

    cpu->pc += instruction.imm;
}

static void bgeu(cpu_t* cpu, memory_t* mem, btype_t instruction) {
    uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);
    uint64_t rs2 = cpu_read_reg(cpu, instruction.rs2);

    if (rs1 < rs2)
        return;

    cpu->pc += instruction.imm;
}

void executeB(cpu_t* cpu, memory_t* mem, btype_t instruction) {
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
