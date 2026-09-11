#include "instructions/dispatchers/load.h"
#include "cpu.h"
#include "exception.h"

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

void dispatch_load(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    switch (instruction.funct3) {
        case 0x0: lb(cpu, mem, instruction); break;
        case 0x1: lh(cpu, mem, instruction); break;
        case 0x2: lw(cpu, mem, instruction); break;
        case 0x3: ld(cpu, mem, instruction); break;
        case 0x4: lbu(cpu, mem, instruction); break;
        case 0x5: lhu(cpu, mem, instruction); break;
        case 0x6: lwu(cpu, mem, instruction); break;
        default: raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
    }
}
