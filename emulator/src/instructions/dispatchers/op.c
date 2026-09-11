#include "instructions/dispatchers/op.h"
#include "cpu.h"
#include "exception.h"

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
    uint8_t shamt = (uint8_t)cpu_read_reg(cpu, instruction.rs2) & 0x3F;
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
    uint8_t shamt = (uint8_t)cpu_read_reg(cpu, instruction.rs2) & 0x3F;
    value >>= shamt;

    cpu_write_reg(cpu, instruction.rd, value);
}

static void sra(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    int64_t value = cpu_read_reg(cpu, instruction.rs1);
    uint8_t shamt = (uint8_t)cpu_read_reg(cpu, instruction.rs2) & 0x3F;
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

void dispatch_op(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    

    switch (instruction.funct3) {
        case 0x0: {
            if (instruction.funct7 == 0b0000000)
                add(cpu, mem, instruction);
            else if (instruction.funct7 == 0b0100000)
                sub(cpu, mem, instruction);
            return;
        }
        case 0x1: sll(cpu, mem, instruction); return;
        case 0x2: slt(cpu, mem, instruction); return;
        case 0x3: sltu(cpu, mem, instruction); return;
        case 0x4: xor_op(cpu, mem, instruction); return;
        case 0x5: {
            if (instruction.funct7 == 0b0000000)
                srl(cpu, mem, instruction);
            else if (instruction.funct7 == 0b0100000)
                sra(cpu, mem, instruction);
            return;
        }
        case 0x6: or_op(cpu, mem, instruction); return;
        case 0x7: and_op(cpu, mem, instruction); return;
        default: raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0); return;
    }

    raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
}


