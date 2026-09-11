#include "instructions/dispatchers/op_imm.h"
#include "cpu.h"
#include "exception.h"

static void addi(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t reg_value = cpu_read_reg(cpu, instruction.rs1);

    cpu_write_reg(cpu, instruction.rd, reg_value + (int64_t)instruction.imm);
}

static void slli(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t operand = cpu_read_reg(cpu, instruction.rs1);
    uint8_t shamt = (uint8_t)(instruction.imm & 0x3F);

    cpu_write_reg(cpu, instruction.rd, operand << shamt);
}

static void slti(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    int64_t rs1 = (int64_t)(cpu_read_reg(cpu, instruction.rs1));
    int64_t imm = (int64_t)(instruction.imm);
    
    if (rs1 < imm)
        cpu_write_reg(cpu, instruction.rd, 1);
    else
        cpu_write_reg(cpu, instruction.rd, 0);
}

static void sltiu(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);
    uint64_t imm = instruction.imm;
    
    if (rs1 < imm)
        cpu_write_reg(cpu, instruction.rd, 1);
    else
        cpu_write_reg(cpu, instruction.rd, 0);
}

static void xori(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);

    cpu_write_reg(cpu, instruction.rd, rs1 ^ instruction.imm);
}

static void srli(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t operand = cpu_read_reg(cpu, instruction.rs1);
    uint8_t shamt = (uint8_t)(instruction.imm & 0x3F);

    cpu_write_reg(cpu, instruction.rd, operand >> shamt);
}

static void srai(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    int64_t operand = (int64_t)(cpu_read_reg(cpu, instruction.rs1));
    uint8_t shamt = (uint8_t)(instruction.imm & 0x3F);
    int64_t result = operand >> shamt;

    cpu_write_reg(cpu, instruction.rd, result);
}

static void sri(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    // imm[11:6] = 0b010000
    if ((instruction.imm >> 6) == 0b010000)
        srai(cpu, mem, instruction);
    else if ((instruction.imm >> 6) == 0b000000)
        srli(cpu, mem, instruction);
    else
        raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
}

static void ori(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);

    cpu_write_reg(cpu, instruction.rd, rs1 | instruction.imm);
}

static void andi(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);

    cpu_write_reg(cpu, instruction.rd, rs1 & instruction.imm);
}

void dispatch_op_imm(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    switch (instruction.funct3) {
        case 0x0: addi(cpu, mem, instruction); break;
        case 0x1: slli(cpu, mem, instruction); break;
        case 0x2: slti(cpu, mem, instruction); break;
        case 0x3: sltiu(cpu, mem, instruction); break;
        case 0x4: xori(cpu, mem, instruction); break;
        case 0x5: sri(cpu, mem, instruction); break;
        case 0x6: ori(cpu, mem, instruction); break;
        case 0x7: andi(cpu, mem, instruction); break;
        default: raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
    }
}
