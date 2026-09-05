#include "instructions/itype.h"
#include "cpu.h"
#include <stdio.h>

static void addi(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t reg_value = cpu_read_reg(cpu, instruction.rs1);

    cpu_write_reg(cpu, instruction.rd, reg_value + instruction.imm);
}

static void addiw(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint32_t rs1 = (uint32_t)(cpu_read_reg(cpu, instruction.rs1));
    uint32_t result = rs1 + (uint32_t)(instruction.imm);

    cpu_write_reg(cpu, instruction.rd, sign_extend(result, 32));
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

static void andi(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);

    cpu_write_reg(cpu, instruction.rd, rs1 & instruction.imm);
}

static void ori(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);

    cpu_write_reg(cpu, instruction.rd, rs1 | instruction.imm);
}

static void xori(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t rs1 = cpu_read_reg(cpu, instruction.rs1);

    cpu_write_reg(cpu, instruction.rd, rs1 ^ instruction.imm);
}

static void slli(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t operand = cpu_read_reg(cpu, instruction.rs1);
    uint8_t shamt = (uint8_t)(instruction.imm & 0x3F);

    cpu_write_reg(cpu, instruction.rd, operand << shamt);
}

static void slliw(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint32_t operand = (uint32_t)(cpu_read_reg(cpu, instruction.rs1));
    uint8_t shamt = (uint8_t)(instruction.imm & 0x1F);

    cpu_write_reg(cpu, instruction.rd, sign_extend(operand << shamt, 32));
}

static void srli(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t operand = cpu_read_reg(cpu, instruction.rs1);
    uint8_t shamt = (uint8_t)(instruction.imm & 0x3F);

    cpu_write_reg(cpu, instruction.rd, operand >> shamt);
}

static void srliw(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint32_t operand = (uint32_t)(cpu_read_reg(cpu, instruction.rs1));
    uint8_t shamt = (uint8_t)(instruction.imm & 0x1F); // bit 5 is reserved

    cpu_write_reg(cpu, instruction.rd, sign_extend(operand >> shamt, 32));
}

static void srai(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    int64_t operand = (int64_t)(cpu_read_reg(cpu, instruction.rs1));
    uint8_t shamt = (uint8_t)(instruction.imm & 0x3F);
    int64_t result = operand >> shamt;

    cpu_write_reg(cpu, instruction.rd, result);
}

static void sraiw(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    int32_t operand = (int32_t)(cpu_read_reg(cpu, instruction.rs1));
    uint8_t shamt = (uint8_t)(instruction.imm & 0x1F); // bit 5 is reserved
    int32_t result = operand >> shamt;

    cpu_write_reg(cpu, instruction.rd, sign_extend(result, 32));
}

static void sri(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    // imm[11:6] = 0b010000
    if ((instruction.imm >> 6) == 0b010000)
        srai(cpu, mem, instruction);
    else if ((instruction.imm >> 6) == 0b000000)
        srli(cpu, mem, instruction);
    else
        fprintf(stderr, "Illegal instruction\n");
}

static void sriw(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    // imm[11:6] = 0b010000
    if ((instruction.imm >> 6) == 0b010000)
        sraiw(cpu, mem, instruction);
    else if ((instruction.imm >> 6) == 0b000000)
        srliw(cpu, mem, instruction);
    else
        fprintf(stderr, "Illegal instruction\n");
}

static void lb(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t addr = instruction.imm + cpu_read_reg(cpu, instruction.rs1);
    uint8_t value = mem_read8(mem, addr);
    cpu_write_reg(cpu, instruction.rd, sign_extend(value, 8));
}

static void lh(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t addr = instruction.imm + cpu_read_reg(cpu, instruction.rs1);
    uint16_t value = mem_read16(mem, addr);
    cpu_write_reg(cpu, instruction.rd, sign_extend(value, 16));
}

static void lw(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t addr = instruction.imm + cpu_read_reg(cpu, instruction.rs1);
    uint32_t value = mem_read32(mem, addr);
    cpu_write_reg(cpu, instruction.rd, sign_extend(value, 32));
}

static void ld(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t addr = instruction.imm + cpu_read_reg(cpu, instruction.rs1);
    uint64_t value = mem_read64(mem, addr);
    cpu_write_reg(cpu, instruction.rd, value);   
}

static void lbu(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t addr = instruction.imm + cpu_read_reg(cpu, instruction.rs1);
    uint8_t value = mem_read8(mem, addr);
    cpu_write_reg(cpu, instruction.rd, value);
}

static void lhu(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t addr = instruction.imm + cpu_read_reg(cpu, instruction.rs1);
    uint16_t value = mem_read16(mem, addr);
    cpu_write_reg(cpu, instruction.rd, value);
}

static void lwu(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t addr = instruction.imm + cpu_read_reg(cpu, instruction.rs1);
    uint32_t value = mem_read32(mem, addr);
    cpu_write_reg(cpu, instruction.rd, value);
}

static void jalr(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t addr = instruction.imm + cpu_read_reg(cpu, instruction.rs1);
    addr &= 0xFFFFFFFFFFFFFFFE; // clear lsb
    cpu_write_reg(cpu, instruction.rd, cpu->pc + 4);
    cpu->pc = addr;
}

static void ecall(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    fprintf(stderr, "ECALL not implemented\n");
}

static void ebreak(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    fprintf(stderr, "EBREAK not implemented\n");
}

static void fence(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    fprintf(stderr, "FENCE not implemented\n");
}

static void dispatch_op_imm(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    switch (instruction.funct3) {
        case 0x0: addi(cpu, mem, instruction); break;
        case 0x1: slli(cpu, mem, instruction); break;
        case 0x2: slti(cpu, mem, instruction); break;
        case 0x3: sltiu(cpu, mem, instruction); break;
        case 0x4: xori(cpu, mem, instruction); break;
        case 0x5: sri(cpu, mem, instruction); break;
        case 0x6: ori(cpu, mem, instruction); break;
        case 0x7: andi(cpu, mem, instruction); break;
        default: fprintf(stderr, "Illegal instruction\n");
    }
}

static void dispatch_op_imm_32(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    switch (instruction.funct3) {
        case 0x0: addiw(cpu, mem, instruction); break;
        case 0x1: slliw(cpu, mem, instruction); break;
        case 0x5: sriw(cpu, mem, instruction); break;
        default: fprintf(stderr, "Illegal instruction\n");
    }
}

static void dispatch_load(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    switch (instruction.funct3) {
        case 0x0: lb(cpu, mem, instruction); break;
        case 0x1: lh(cpu, mem, instruction); break;
        case 0x2: lw(cpu, mem, instruction); break;
        case 0x3: ld(cpu, mem, instruction); break;
        case 0x4: lbu(cpu, mem, instruction); break;
        case 0x5: lhu(cpu, mem, instruction); break;
        case 0x6: lwu(cpu, mem, instruction); break;
        default: fprintf(stderr, "Illegal instruction\n");
    }
}

static void dispatch_system(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    if (instruction.rs1 != 0 || instruction.rd != 0 || instruction.funct3 != 0) {
        fprintf(stderr, "Illegal instruction\n");
        return;
    } 

    switch (instruction.imm) {
        case 0x0: ecall(cpu, mem, instruction); break;
        case 0x1: ebreak(cpu, mem, instruction); break;
        default: fprintf(stderr, "Illegal instruction\n");
    }
}

void executeI(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    opcode_t opcode = (opcode_t)instruction.opcode;
    
    if (opcode == OP_IMM)
        dispatch_op_imm(cpu, mem, instruction);
    else if (opcode == OP_IMM_32)
        dispatch_op_imm_32(cpu, mem, instruction);
    else if (opcode == LOAD)
        dispatch_load(cpu, mem, instruction);
    else if (opcode == JALR)
        jalr(cpu, mem, instruction);
    else if (opcode == SYSTEM)
        dispatch_system(cpu, mem, instruction);
    else if (opcode == MISC_MEM)
        fence(cpu, mem, instruction);
    else
        fprintf(stderr, "Illegal instruction\n");
    
}
