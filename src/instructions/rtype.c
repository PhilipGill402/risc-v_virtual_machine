#include "instructions/rtype.h"
#include "cpu.h"

#include <stdio.h>

static void add(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    uint64_t op1 = cpu_read_reg(cpu, instruction.rs1);
    uint64_t op2 = cpu_read_reg(cpu, instruction.rs2);

    cpu_write_reg(cpu, instruction.rd, op1 + op2);
}

static void sub(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    uint64_t op1 = cpu_read_reg(cpu, instruction.rs1);
    uint64_t op2 = cpu_read_reg(cpu, instruction.rs2);

    cpu_write_reg(cpu, instruction.rd, op1 - op2);
}


static void sll(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    uint64_t value = cpu_read_reg(cpu, instruction.rs1);
    uint8_t shamt = (uint8_t)instruction.rs2 & 0x1F;
    value <<= shamt;

    cpu_write_reg(cpu, instruction.rd, value);
}

static void slt(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    int64_t rs1 = (int64_t)cpu_read_reg(cpu, instruction.rs1);
    int64_t rs2 = (int64_t)cpu_read_reg(cpu, instruction.rs2);

    cpu_write_reg(cpu, instruction.rd, rs1 < rs2);
}

static void sltu(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);
    uint64_t rs2 = cpu_read_reg(cpu, instruction.rs2);

    cpu_write_reg(cpu, instruction.rd, rs1 < rs2);
}

static void xor_op(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);
    uint64_t rs2 = cpu_read_reg(cpu, instruction.rs2);

    cpu_write_reg(cpu, instruction.rd, rs1 ^ rs2);
}

static void srl(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    uint64_t value = cpu_read_reg(cpu, instruction.rs1);
    uint8_t shamt = (uint8_t)instruction.rs2 & 0x1F;
    value >>= shamt;

    cpu_write_reg(cpu, instruction.rd, value);
}

static void sra(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    int64_t value = cpu_read_reg(cpu, instruction.rs1);
    uint8_t shamt = (uint8_t)instruction.rs2 & 0x1F;
    value >>= shamt;

    cpu_write_reg(cpu, instruction.rd, (uint64_t)value);
}

static void or_op(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);
    uint64_t rs2 = cpu_read_reg(cpu, instruction.rs2);

    cpu_write_reg(cpu, instruction.rd, rs1 | rs2);
}

static void and_op(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);
    uint64_t rs2 = cpu_read_reg(cpu, instruction.rs2);

    cpu_write_reg(cpu, instruction.rd, rs1 & rs2);
}

static void addw(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    int32_t rs1 = cpu_read_reg(cpu, instruction.rs1);
    int32_t rs2 = cpu_read_reg(cpu, instruction.rs2);

    cpu_write_reg(cpu, instruction.rd, sign_extend(rs1 + rs2, 32));
}

static void subw(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    int32_t rs1 = cpu_read_reg(cpu, instruction.rs1);
    int32_t rs2 = cpu_read_reg(cpu, instruction.rs2);

    cpu_write_reg(cpu, instruction.rd, sign_extend(rs1 - rs2, 32));
}

static void sllw(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    uint32_t value = (uint32_t)cpu_read_reg(cpu, instruction.rs1);
    uint8_t shamt = (uint8_t)instruction.rs2 & 0xF;
    value <<= shamt;

    cpu_write_reg(cpu, instruction.rd, sign_extend(value, 32));
}

static void srlw(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    uint32_t value = (uint32_t)cpu_read_reg(cpu, instruction.rs1);
    uint8_t shamt = (uint8_t)instruction.rs2 & 0xF;
    value >>= shamt;

    cpu_write_reg(cpu, instruction.rd, sign_extend(value, 32));
}

static void sraw(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    int32_t value = (int32_t)cpu_read_reg(cpu, instruction.rs1);
    uint8_t shamt = (uint8_t)instruction.rs2 & 0xF;
    value >>= shamt;

    cpu_write_reg(cpu, instruction.rd, sign_extend((uint32_t)value, 32));}

static void dispatch_op(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
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
        case 0x5: {
            if (instruction.funct7 == 0b0000000)
                srl(cpu, mem, instruction);
            else if (instruction.funct7 == 0b0100000)
                sra(cpu, mem, instruction);
            break;
        }
        case 0x6: or_op(cpu, mem, instruction); break;
        case 0x7: and_op(cpu, mem, instruction); break;
        default: fprintf(stderr, "Illegal instruction\n");
    }
}

static void dispatch_op_32(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
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

void executeR(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    opcode_t opcode = (opcode_t)instruction.opcode;
    
    if (opcode == OP)
        dispatch_op(cpu, mem, instruction);
    else if (opcode == OP_32)
        dispatch_op_32(cpu, mem, instruction);
    else
        fprintf(stderr, "Illegal instruction\n");
}



