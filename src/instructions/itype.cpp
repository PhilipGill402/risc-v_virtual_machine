#include "instructions/itype.h"
#include "cpu.h"
#include <stdio.h>

static void addi(CPU& cpu, IType instruction) {
    uint64_t reg_value = cpu.read_reg(instruction.rs1);

    cpu.write_reg(instruction.rd, reg_value + instruction.imm);
}

static void addiw(CPU& cpu, IType instruction) {
    uint32_t rs1 = static_cast<uint32_t>(cpu.read_reg(instruction.rs1));
    uint32_t result = rs1 + static_cast<uint32_t>(instruction.imm);

    cpu.write_reg(instruction.rd, sign_extend(result, 32));
}

static void slti(CPU& cpu, IType instruction) {
    int64_t rs1 = static_cast<int64_t>(cpu.read_reg(instruction.rs1));
    int64_t imm = static_cast<int64_t>(instruction.imm);
    
    if (rs1 < imm)
        cpu.write_reg(instruction.rd, 1);
    else
        cpu.write_reg(instruction.rd, 0);
}

static void sltiu(CPU& cpu, IType instruction) {
    uint64_t rs1 = cpu.read_reg(instruction.rs1);
    uint64_t imm = instruction.imm;
    
    if (rs1 < imm)
        cpu.write_reg(instruction.rd, 1);
    else
        cpu.write_reg(instruction.rd, 0);
}

static void andi(CPU& cpu, IType instruction) {
    uint64_t rs1 = cpu.read_reg(instruction.rs1);

    cpu.write_reg(instruction.rd, rs1 & instruction.imm);
}

static void ori(CPU& cpu, IType instruction) {
    uint64_t rs1 = cpu.read_reg(instruction.rs1);

    cpu.write_reg(instruction.rd, rs1 | instruction.imm);
}

static void xori(CPU& cpu, IType instruction) {
    uint64_t rs1 = cpu.read_reg(instruction.rs1);

    cpu.write_reg(instruction.rd, rs1 ^ instruction.imm);
}

static void slli(CPU& cpu, IType instruction) {
    uint64_t operand = cpu.read_reg(instruction.rs1);
    uint8_t shamt = static_cast<uint8_t>(instruction.imm & 0x3F);

    cpu.write_reg(instruction.rd, operand << shamt);
}

static void slliw(CPU& cpu, IType instruction) {
    uint32_t operand = static_cast<uint32_t>(cpu.read_reg(instruction.rs1));
    uint8_t shamt = static_cast<uint8_t>(instruction.imm & 0x1F);

    cpu.write_reg(instruction.rd, sign_extend(operand << shamt, 32));
}

static void srli(CPU& cpu, IType instruction) {
    uint64_t operand = cpu.read_reg(instruction.rs1);
    uint8_t shamt = static_cast<uint8_t>(instruction.imm & 0x3F);

    cpu.write_reg(instruction.rd, operand >> shamt);
}

static void srliw(CPU& cpu, IType instruction) {
    uint32_t operand = static_cast<uint32_t>(cpu.read_reg(instruction.rs1));
    uint8_t shamt = static_cast<uint8_t>(instruction.imm & 0x1F); // bit 5 is reserved

    cpu.write_reg(instruction.rd, sign_extend(operand >> shamt, 32));
}

static void srai(CPU& cpu, IType instruction) {
    int64_t operand = static_cast<int64_t>(cpu.read_reg(instruction.rs1));
    uint8_t shamt = static_cast<uint8_t>(instruction.imm & 0x3F);
    int64_t result = operand >> shamt;

    cpu.write_reg(instruction.rd, result);
}

static void sraiw(CPU& cpu, IType instruction) {
    int32_t operand = static_cast<int32_t>(cpu.read_reg(instruction.rs1));
    uint8_t shamt = static_cast<uint8_t>(instruction.imm & 0x1F); // bit 5 is reserved
    int32_t result = operand >> shamt;

    cpu.write_reg(instruction.rd, sign_extend(result, 32));
}

static void sri(CPU& cpu, IType instruction) {
    // imm[11:6] = 0b010000
    if ((instruction.imm >> 6) == 0b010000)
        srai(cpu, instruction);
    else if ((instruction.imm >> 6) == 0b000000)
        srli(cpu, instruction);
    else
        fprintf(stderr, "Illegal instruction\n");
}

static void sriw(CPU& cpu, IType instruction) {
    // imm[11:6] = 0b010000
    if ((instruction.imm >> 6) == 0b010000)
        sraiw(cpu, instruction);
    else if ((instruction.imm >> 6) == 0b000000)
        srliw(cpu, instruction);
    else
        fprintf(stderr, "Illegal instruction\n");
}

static void dispatch_op_imm(CPU& cpu, IType instruction) {
    switch (instruction.funct3) {
        case 0x0: addi(cpu, instruction); break;
        case 0x1: slli(cpu, instruction); break;
        case 0x2: slti(cpu, instruction); break;
        case 0x3: sltiu(cpu, instruction); break;
        case 0x4: xori(cpu, instruction); break;
        case 0x5: sri(cpu, instruction); break;
        case 0x6: ori(cpu, instruction); break;
        case 0x7: andi(cpu, instruction); break;
        default: fprintf(stderr, "Illegal instruction\n");
    }
}

static void dispatch_op_imm_32(CPU& cpu, IType instruction) {
    switch (instruction.funct3) {
        case 0x0: addiw(cpu, instruction); break;
        case 0x1: slliw(cpu, instruction); break;
        case 0x5: sriw(cpu, instruction); break;
        default: fprintf(stderr, "Illegal instruction\n");
    }
}

void executeI(CPU& cpu, IType instruction) {
    Opcode opcode = static_cast<Opcode>(instruction.opcode);
    
    if (opcode == Opcode::OP_IMM)
        dispatch_op_imm(cpu, instruction);
    else if (opcode == Opcode::OP_IMM_32)
        dispatch_op_imm_32(cpu, instruction);
    else
        fprintf(stderr, "Illegal instruction\n");
    
}
