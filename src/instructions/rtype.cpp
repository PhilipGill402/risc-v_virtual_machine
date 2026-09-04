#include "instructions/rtype.h"
#include "cpu.h"

#include <stdio.h>

static void add(CPU& cpu, Memory& mem, RType instruction) {
    uint64_t op1 = cpu.read_reg(instruction.rs1);
    uint64_t op2 = cpu.read_reg(instruction.rs2);

    cpu.write_reg(instruction.rd, op1 + op2);
}

static void sub(CPU& cpu, Memory& mem, RType instruction) {
    uint64_t op1 = cpu.read_reg(instruction.rs1);
    uint64_t op2 = cpu.read_reg(instruction.rs2);

    cpu.write_reg(instruction.rd, op1 - op2);
}

static void addw(CPU& cpu, Memory& mem, RType instruction) {
    int32_t rs1 = cpu.read_reg(instruction.rs1);
    int32_t rs2 = cpu.read_reg(instruction.rs2);

    cpu.write_reg(instruction.rd, sign_extend(rs1 + rs2, 32));
}

static void subw(CPU& cpu, Memory& mem, RType instruction) {
    int32_t rs1 = cpu.read_reg(instruction.rs1);
    int32_t rs2 = cpu.read_reg(instruction.rs2);

    cpu.write_reg(instruction.rd, sign_extend(rs1 - rs2, 32));
}

static void sll(CPU& cpu, Memory& mem, RType instruction) {
    uint64_t value = cpu.read_reg(instruction.rs1);
    uint8_t shamt = static_cast<uint8_t>(instruction.rs2) & 0x1F;
    value <<= shamt;

    cpu.write_reg(instruction.rd, value);
}

static void slt(CPU& cpu, Memory& mem, RType instruction) {
    int64_t rs1 = static_cast<int64_t>(cpu.read_reg(instruction.rs1));
    int64_t rs2 = static_cast<int64_t>(cpu.read_reg(instruction.rs2));

    cpu.write_reg(instruction.rd, rs1 < rs2);
}

static void sltu(CPU& cpu, Memory& mem, RType instruction) {
    uint64_t rs1 = cpu.read_reg(instruction.rs1);
    uint64_t rs2 = cpu.read_reg(instruction.rs2);

    cpu.write_reg(instruction.rd, rs1 < rs2);
}

static void xor_op(CPU& cpu, Memory& mem, RType instruction) {
    uint64_t rs1 = cpu.read_reg(instruction.rs1);
    uint64_t rs2 = cpu.read_reg(instruction.rs2);

    cpu.write_reg(instruction.rd, rs1 ^ rs2);
}

static void srl(CPU& cpu, Memory& mem, RType instruction) {
    uint64_t value = cpu.read_reg(instruction.rs1);
    uint8_t shamt = static_cast<uint8_t>(instruction.rs2) & 0x1F;
    value >>= shamt;

    cpu.write_reg(instruction.rd, value);
}

static void or_op(CPU& cpu, Memory& mem, RType instruction) {
    uint64_t rs1 = cpu.read_reg(instruction.rs1);
    uint64_t rs2 = cpu.read_reg(instruction.rs2);

    cpu.write_reg(instruction.rd, rs1 | rs2);
}

static void and_op(CPU& cpu, Memory& mem, RType instruction) {
    uint64_t rs1 = cpu.read_reg(instruction.rs1);
    uint64_t rs2 = cpu.read_reg(instruction.rs2);

    cpu.write_reg(instruction.rd, rs1 & rs2);
}

static void dispatch_op(CPU& cpu, Memory& mem, RType instruction) {
    if (instruction.funct7 != 0b0000000 && instruction.funct3 != 0x0) {
        fprintf(stderr, "Illegal instruction\n");
        return;
    } 

    switch (instruction.funct3) {
        case 0x0: {
            if (instruction.funct7 == 0b0000000)
                add(cpu, mem, instruction);
            else if (instruction.funct7 == 0b0100000)
                sub(cpu, mem, instruction);
            break;
        }
        case 0x1: sll(cpu, mem, instruction); break;
        case 0x2: slt(cpu, mem, instruction); break;
        case 0x3: sltu(cpu, mem, instruction); break;
        case 0x4: xor_op(cpu, mem, instruction); break;
        case 0x5: srl(cpu, mem, instruction); break;
        case 0x6: or_op(cpu, mem, instruction); break;
        case 0x7: and_op(cpu, mem, instruction); break;
        default: fprintf(stderr, "Illegal instruction\n");
    }
}

static void dispatch_op_32(CPU& cpu, Memory& mem, RType instruction) {
    if (instruction.funct7 != 0b0000000 && instruction.funct3 != 0x0) {
        fprintf(stderr, "Illegal instruction\n");
        return;
    } 

    switch (instruction.funct3) {
        case 0x0: {
            if (instruction.funct7 == 0b0000000)
                addw(cpu, mem, instruction);
            else if (instruction.funct7 == 0b0100000)
                subw(cpu, mem, instruction);
            break;
        }
        case 0x1: sllw(cpu, mem, instruction); break;
        case 0x5: {
            if (instruction.funct7 == 0b0000000)
                srlw(cpu, mem, instruction);
            else if (instruction.funct7 == 0b0100000)
                sraw(cpu, mem, instruction);
            break;
        }; 
        default: fprintf(stderr, "Illegal instruction\n");
    }
}

void executeR(CPU& cpu, Memory& mem, RType instruction) {
    Opcode opcode = static_cast<Opcode>(instruction.opcode);
    
    if (opcode == Opcode::OP)
        dispatch_op(cpu, mem, instruction);
    else if (opcode == Opcode::OP_32)
        dispatch_op_32(cpu, mem, instruction);
    else
        fprintf(stderr, "Illegal instruction\n");
}



