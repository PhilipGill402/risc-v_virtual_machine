#include "cpu/instructions/dispatchers/branch.h"
#include "cpu/trap.h"
#include "cpu/cpu.h"

static void beq(cpu_t* cpu, btype_t instruction) {
    uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);
    uint64_t rs2 = cpu_read_reg(cpu, instruction.rs2);

    if (rs1 == rs2) {
        cpu->pc += (int64_t)instruction.imm;
        cpu->pc_written = 1;
    }
}

static void bne(cpu_t* cpu, btype_t instruction) {
    uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);
    uint64_t rs2 = cpu_read_reg(cpu, instruction.rs2);

    if (rs1 != rs2) {
        cpu->pc += (int64_t)instruction.imm;
        cpu->pc_written = 1;
    }
}

static void blt(cpu_t* cpu, btype_t instruction) {
    int64_t rs1 = (int64_t)cpu_read_reg(cpu, instruction.rs1);
    int64_t rs2 = (int64_t)cpu_read_reg(cpu, instruction.rs2);

    if (rs1 < rs2) {
        cpu->pc += (int64_t)instruction.imm;
        cpu->pc_written = 1;
    }
}

static void bge(cpu_t* cpu, btype_t instruction) {
    int64_t rs1 = (int64_t)cpu_read_reg(cpu, instruction.rs1);
    int64_t rs2 = (int64_t)cpu_read_reg(cpu, instruction.rs2);

    if (rs1 >= rs2) {
        cpu->pc += (int64_t)instruction.imm;
        cpu->pc_written = 1;
    }
}

static void bltu(cpu_t* cpu, btype_t instruction) {
    uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);
    uint64_t rs2 = cpu_read_reg(cpu, instruction.rs2);

    if (rs1 < rs2) {
        cpu->pc += instruction.imm;
        cpu->pc_written = 1;
    }
}

static void bgeu(cpu_t* cpu, btype_t instruction) {
    uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);
    uint64_t rs2 = cpu_read_reg(cpu, instruction.rs2);

    if (rs1 >= rs2) {
        cpu->pc += instruction.imm;
        cpu->pc_written = 1;
    }
}

void dispatch_branch(cpu_t* cpu, btype_t instruction) {
    switch(instruction.funct3) {
        case 0x0: beq(cpu, instruction); break;
        case 0x1: bne(cpu, instruction); break;
        case 0x4: blt(cpu, instruction); break;
        case 0x5: bge(cpu, instruction); break;
        case 0x6: bltu(cpu, instruction); break;
        case 0x7: bgeu(cpu, instruction); break;
        default: raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, instruction.raw);
    }
}
