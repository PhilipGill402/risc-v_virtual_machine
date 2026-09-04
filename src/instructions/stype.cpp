#include "instructions/stype.h"
#include "cpu.h"
#include <stdio.h>

static void sb(CPU& cpu, Memory& mem, SType instruction) {
    uint8_t value = static_cast<uint8_t>(cpu.read_reg(instruction.rs2));
    uint64_t addr = cpu.read_reg(instruction.rs1) + static_cast<int64_t>(instruction.imm);

    mem.write8(value, addr);
}

static void sh(CPU& cpu, Memory& mem, SType instruction) {
    uint16_t value = static_cast<uint16_t>(cpu.read_reg(instruction.rs2));
    uint64_t addr = cpu.read_reg(instruction.rs1) + static_cast<int64_t>(instruction.imm);

    mem.write16(value, addr);
}

static void sw(CPU& cpu, Memory& mem, SType instruction) {
    uint32_t value = static_cast<uint32_t>(cpu.read_reg(instruction.rs2));
    uint64_t addr = cpu.read_reg(instruction.rs1) + static_cast<int64_t>(instruction.imm);

    mem.write32(value, addr);
}

static void sd(CPU& cpu, Memory& mem, SType instruction) {
    uint64_t value = static_cast<uint64_t>(cpu.read_reg(instruction.rs2));
    uint64_t addr = cpu.read_reg(instruction.rs1) + static_cast<int64_t>(instruction.imm);

    mem.write64(value, addr);
}

void executeS(CPU& cpu, Memory& mem, SType instruction) {
    switch (instruction.funct3) {
        case 0x0: sb(cpu, mem, instruction); break;
        case 0x1: sh(cpu, mem, instruction); break;
        case 0x2: sw(cpu, mem, instruction); break;
        case 0x3: sd(cpu, mem, instruction); break;
        default: fprintf(stderr, "Illegal instruction\n");
    }
}
