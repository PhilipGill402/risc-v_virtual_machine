#include "instructions/itype.h"
#include "cpu.h"
#include <stdio.h>

static void addi(CPU& cpu, Memory& mem, IType instruction) {
    uint64_t reg_value = cpu.read_reg(instruction.rs1);

    cpu.write_reg(instruction.rd, reg_value + instruction.imm);
}

static void addiw(CPU& cpu, Memory& mem, IType instruction) {
    uint32_t rs1 = static_cast<uint32_t>(cpu.read_reg(instruction.rs1));
    uint32_t result = rs1 + static_cast<uint32_t>(instruction.imm);

    cpu.write_reg(instruction.rd, sign_extend(result, 32));
}

static void slti(CPU& cpu, Memory& mem, IType instruction) {
    int64_t rs1 = static_cast<int64_t>(cpu.read_reg(instruction.rs1));
    int64_t imm = static_cast<int64_t>(instruction.imm);
    
    if (rs1 < imm)
        cpu.write_reg(instruction.rd, 1);
    else
        cpu.write_reg(instruction.rd, 0);
}

static void sltiu(CPU& cpu, Memory& mem, IType instruction) {
    uint64_t rs1 = cpu.read_reg(instruction.rs1);
    uint64_t imm = instruction.imm;
    
    if (rs1 < imm)
        cpu.write_reg(instruction.rd, 1);
    else
        cpu.write_reg(instruction.rd, 0);
}

static void andi(CPU& cpu, Memory& mem, IType instruction) {
    uint64_t rs1 = cpu.read_reg(instruction.rs1);

    cpu.write_reg(instruction.rd, rs1 & instruction.imm);
}

static void ori(CPU& cpu, Memory& mem, IType instruction) {
    uint64_t rs1 = cpu.read_reg(instruction.rs1);

    cpu.write_reg(instruction.rd, rs1 | instruction.imm);
}

static void xori(CPU& cpu, Memory& mem, IType instruction) {
    uint64_t rs1 = cpu.read_reg(instruction.rs1);

    cpu.write_reg(instruction.rd, rs1 ^ instruction.imm);
}

static void slli(CPU& cpu, Memory& mem, IType instruction) {
    uint64_t operand = cpu.read_reg(instruction.rs1);
    uint8_t shamt = static_cast<uint8_t>(instruction.imm & 0x3F);

    cpu.write_reg(instruction.rd, operand << shamt);
}

static void slliw(CPU& cpu, Memory& mem, IType instruction) {
    uint32_t operand = static_cast<uint32_t>(cpu.read_reg(instruction.rs1));
    uint8_t shamt = static_cast<uint8_t>(instruction.imm & 0x1F);

    cpu.write_reg(instruction.rd, sign_extend(operand << shamt, 32));
}

static void srli(CPU& cpu, Memory& mem, IType instruction) {
    uint64_t operand = cpu.read_reg(instruction.rs1);
    uint8_t shamt = static_cast<uint8_t>(instruction.imm & 0x3F);

    cpu.write_reg(instruction.rd, operand >> shamt);
}

static void srliw(CPU& cpu, Memory& mem, IType instruction) {
    uint32_t operand = static_cast<uint32_t>(cpu.read_reg(instruction.rs1));
    uint8_t shamt = static_cast<uint8_t>(instruction.imm & 0x1F); // bit 5 is reserved

    cpu.write_reg(instruction.rd, sign_extend(operand >> shamt, 32));
}

static void srai(CPU& cpu, Memory& mem, IType instruction) {
    int64_t operand = static_cast<int64_t>(cpu.read_reg(instruction.rs1));
    uint8_t shamt = static_cast<uint8_t>(instruction.imm & 0x3F);
    int64_t result = operand >> shamt;

    cpu.write_reg(instruction.rd, result);
}

static void sraiw(CPU& cpu, Memory& mem, IType instruction) {
    int32_t operand = static_cast<int32_t>(cpu.read_reg(instruction.rs1));
    uint8_t shamt = static_cast<uint8_t>(instruction.imm & 0x1F); // bit 5 is reserved
    int32_t result = operand >> shamt;

    cpu.write_reg(instruction.rd, sign_extend(result, 32));
}

static void sri(CPU& cpu, Memory& mem, IType instruction) {
    // imm[11:6] = 0b010000
    if ((instruction.imm >> 6) == 0b010000)
        srai(cpu, mem, instruction);
    else if ((instruction.imm >> 6) == 0b000000)
        srli(cpu, mem, instruction);
    else
        fprintf(stderr, "Illegal instruction\n");
}

static void sriw(CPU& cpu, Memory& mem, IType instruction) {
    // imm[11:6] = 0b010000
    if ((instruction.imm >> 6) == 0b010000)
        sraiw(cpu, mem, instruction);
    else if ((instruction.imm >> 6) == 0b000000)
        srliw(cpu, mem, instruction);
    else
        fprintf(stderr, "Illegal instruction\n");
}

static void lb(CPU& cpu, Memory& mem, IType instruction) {
    uint64_t addr = instruction.imm + cpu.read_reg(instruction.rs1);
    uint8_t value = mem.read8(addr);
    cpu.write_reg(instruction.rd, sign_extend(value, 8));
}

static void lh(CPU& cpu, Memory& mem, IType instruction) {
    uint64_t addr = instruction.imm + cpu.read_reg(instruction.rs1);
    uint16_t value = mem.read16(addr);
    cpu.write_reg(instruction.rd, sign_extend(value, 16));
}

static void lw(CPU& cpu, Memory& mem, IType instruction) {
    uint64_t addr = instruction.imm + cpu.read_reg(instruction.rs1);
    uint32_t value = mem.read32(addr);
    cpu.write_reg(instruction.rd, sign_extend(value, 32));
}

static void ld(CPU& cpu, Memory& mem, IType instruction) {
    uint64_t addr = instruction.imm + cpu.read_reg(instruction.rs1);
    uint64_t value = mem.read64(addr);
    cpu.write_reg(instruction.rd, sign_extend(value, 64));   
}

static void lbu(CPU& cpu, Memory& mem, IType instruction) {
    uint64_t addr = instruction.imm + cpu.read_reg(instruction.rs1);
    uint8_t value = mem.read8(addr);
    cpu.write_reg(instruction.rd, value);
}

static void lhu(CPU& cpu, Memory& mem, IType instruction) {
    uint64_t addr = instruction.imm + cpu.read_reg(instruction.rs1);
    uint16_t value = mem.read16(addr);
    cpu.write_reg(instruction.rd, value);
}

static void lwu(CPU& cpu, Memory& mem, IType instruction) {
    uint64_t addr = instruction.imm + cpu.read_reg(instruction.rs1);
    uint32_t value = mem.read32(addr);
    cpu.write_reg(instruction.rd, value);
}

static void jalr(CPU& cpu, Memory& mem, IType instruction) {
    uint64_t addr = instruction.imm + cpu.read_reg(instruction.rs1);
    addr &= 0xFFFFFFFFFFFFFFFE; // clear lsb
    cpu.write_reg(instruction.rd, cpu.pc + 4);
    cpu.pc = addr;
}

static void ecall(CPU& cpu, Memory& mem, IType instruction) {
    fprintf(stderr, "ECALL not implemented\n");
}

static void ebreak(CPU& cpu, Memory& mem, IType instruction) {
    fprintf(stderr, "EBREAK not implemented\n");
}

static void fence(CPU& cpu, Memory& mem, IType instruction) {
    fprintf(stderr, "FENCE not implemented\n");
}

static void dispatch_op_imm(CPU& cpu, Memory& mem, IType instruction) {
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

static void dispatch_op_imm_32(CPU& cpu, Memory& mem, IType instruction) {
    switch (instruction.funct3) {
        case 0x0: addiw(cpu, mem, instruction); break;
        case 0x1: slliw(cpu, mem, instruction); break;
        case 0x5: sriw(cpu, mem, instruction); break;
        default: fprintf(stderr, "Illegal instruction\n");
    }
}

static void dispatch_load(CPU& cpu, Memory& mem, IType instruction) {
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

static void dispatch_system(CPU& cpu, Memory& mem, IType instruction) {
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

void executeI(CPU& cpu, Memory& mem, IType instruction) {
    Opcode opcode = static_cast<Opcode>(instruction.opcode);
    
    if (opcode == Opcode::OP_IMM)
        dispatch_op_imm(cpu, mem, instruction);
    else if (opcode == Opcode::OP_IMM_32)
        dispatch_op_imm_32(cpu, mem, instruction);
    else if (opcode == Opcode::LOAD)
        dispatch_load(cpu, mem, instruction);
    else if (opcode == Opcode::JALR)
        jalr(cpu, mem, instruction);
    else if (opcode == Opcode::SYSTEM)
        dispatch_system(cpu, mem, instruction);
    else if (opcode == Opcode::MISC_MEM)
        fence(cpu, mem, instruction);
    else
        fprintf(stderr, "Illegal instruction\n");
    
}
