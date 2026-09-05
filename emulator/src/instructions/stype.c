#include "instructions/stype.h"
#include "cpu.h"
#include <stdio.h>

static void sb(cpu_t* cpu, memory_t* mem, stype_t instruction) {
    uint8_t value = (uint8_t)cpu_read_reg(cpu, instruction.rs2);
    uint64_t addr = cpu_read_reg(cpu, instruction.rs1) + (int64_t)instruction.imm;

    mem_write8(mem, addr, value);
}

static void sh(cpu_t* cpu, memory_t* mem, stype_t instruction) {
    uint16_t value = (uint16_t)cpu_read_reg(cpu, instruction.rs2);
    uint64_t addr = cpu_read_reg(cpu, instruction.rs1) + (int64_t)instruction.imm;

    mem_write16(mem, addr, value);
}

static void sw(cpu_t* cpu, memory_t* mem, stype_t instruction) {
    uint32_t value = (uint32_t)cpu_read_reg(cpu, instruction.rs2);
    uint64_t addr = cpu_read_reg(cpu, instruction.rs1) + (int64_t)instruction.imm;

    mem_write32(mem, addr, value);
}

static void sd(cpu_t* cpu, memory_t* mem, stype_t instruction) {
    uint64_t value = (uint64_t)cpu_read_reg(cpu, instruction.rs2);
    uint64_t addr = cpu_read_reg(cpu, instruction.rs1) + (int64_t)instruction.imm;
    
    mem_write64(mem, addr, value);
}

void executeS(cpu_t* cpu, memory_t* mem, stype_t instruction) {
    switch (instruction.funct3) {
        case 0x0: sb(cpu, mem, instruction); break;
        case 0x1: sh(cpu, mem, instruction); break;
        case 0x2: sw(cpu, mem, instruction); break;
        case 0x3: sd(cpu, mem, instruction); break;
        default: fprintf(stderr, "Illegal instruction\n");
    }
}
