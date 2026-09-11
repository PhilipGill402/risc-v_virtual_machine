#include "instructions/dispatchers/op_imm_32.h"
#include "cpu.h"
#include "exception.h"

static void addiw(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint32_t rs1 = (uint32_t)(cpu_read_reg(cpu, instruction.rs1));
    uint32_t result = rs1 + (int32_t)(instruction.imm);

    cpu_write_reg(cpu, instruction.rd, sign_extend(result, 32));
}

static void slliw(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint32_t operand = (uint32_t)(cpu_read_reg(cpu, instruction.rs1));
    uint8_t shamt = (uint8_t)(instruction.imm & 0x1F);

    cpu_write_reg(cpu, instruction.rd, sign_extend(operand << shamt, 32));
}

static void sraiw(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    int32_t operand = (int32_t)(cpu_read_reg(cpu, instruction.rs1));
    uint8_t shamt = (uint8_t)(instruction.imm & 0x1F); // bit 5 is reserved
    int32_t result = operand >> shamt;

    cpu_write_reg(cpu, instruction.rd, sign_extend(result, 32));
}

static void srliw(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint32_t operand = (uint32_t)(cpu_read_reg(cpu, instruction.rs1));
    uint8_t shamt = (uint8_t)(instruction.imm & 0x1F); // bit 5 is reserved

    cpu_write_reg(cpu, instruction.rd, sign_extend(operand >> shamt, 32));
}

static void sriw(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    // imm[11:6] = 0b010000
    if ((instruction.imm >> 6) == 0b010000)
        sraiw(cpu, mem, instruction);
    else if ((instruction.imm >> 6) == 0b000000)
        srliw(cpu, mem, instruction);
    else
        raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
}

void dispatch_op_imm_32(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    switch (instruction.funct3) {
        case 0x0: addiw(cpu, mem, instruction); break;
        case 0x1: slliw(cpu, mem, instruction); break;
        case 0x5: sriw(cpu, mem, instruction); break;
        default: raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
    }
}
