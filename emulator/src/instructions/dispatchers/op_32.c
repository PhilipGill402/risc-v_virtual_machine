#include "instructions/dispatchers/op_32.h"
#include "cpu.h"
#include "exception.h"

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
    uint8_t shamt = (uint8_t)cpu_read_reg(cpu, instruction.rs2) & 0x1F;
    value <<= shamt;

    cpu_write_reg(cpu, instruction.rd, sign_extend(value, 32));
}

static void srlw(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    uint32_t value = (uint32_t)cpu_read_reg(cpu, instruction.rs1);
    uint8_t shamt = (uint8_t)cpu_read_reg(cpu, instruction.rs2) & 0x1F;
    value >>= shamt;

    cpu_write_reg(cpu, instruction.rd, sign_extend(value, 32));
}

static void sraw(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    int32_t value = (int32_t)cpu_read_reg(cpu, instruction.rs1);
    uint8_t shamt = (uint8_t)cpu_read_reg(cpu, instruction.rs2) & 0xF;
    value >>= shamt;

    cpu_write_reg(cpu, instruction.rd, sign_extend((uint32_t)value, 32));
}

void dispatch_op_32(cpu_t* cpu, memory_t* mem, rtype_t instruction) {
    switch (instruction.funct3) {
        case 0x0: {
            if (instruction.funct7 == 0b0000000)
                addw(cpu, mem, instruction);
            else if (instruction.funct7 == 0b0100000)
                subw(cpu, mem, instruction);
            return;
        }
        case 0x1: sllw(cpu, mem, instruction); return;
        case 0x5: {
            if (instruction.funct7 == 0b0000000)
                srlw(cpu, mem, instruction);
            else if (instruction.funct7 == 0b0100000)
                sraw(cpu, mem, instruction);
            return;
        }; 
        default: raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0); return;
    }

    raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
}

