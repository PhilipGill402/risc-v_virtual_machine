#include "cpu.h"
#include "instructions/decoding.h"
#include "instructions/itype.h"
#include "instructions/jtype.h"
#include "instructions/rtype.h"
#include "instructions/stype.h"
#include "instructions/utype.h"
#include "instructions/btype.h"
#include <stdio.h>

cpu_t cpu_init() {
    cpu_t cpu = { 0 };
    
    return cpu;
}

void cpu_reset(cpu_t* cpu) {
    cpu->pc = MEM_BASE;
}

uint32_t cpu_fetch(cpu_t* cpu, memory_t* mem) {
    return mem_read32(mem, cpu->pc);
}

void cpu_write_reg(cpu_t* cpu, uint8_t reg_num, uint64_t value) {
    if (reg_num == 0)
        return;
    
    if (reg_num >= 32) {
        fprintf(stderr, "Register number out of range: %u\n", reg_num);
        return;
    }

    cpu->regs[reg_num] = value;
}

uint64_t cpu_read_reg(cpu_t* cpu, uint8_t reg_num) {
    if (reg_num == 0)
        return 0;

    if (reg_num >= 32) {
        fprintf(stderr, "Register number out of range: %u\n", reg_num);
        return 0;
    }

    return cpu->regs[reg_num];
}

void cpu_step(cpu_t* cpu, memory_t* mem) {
    uint32_t instruction = cpu_fetch(cpu, mem);

    uint8_t raw_opcode = instruction & 0x7F;
    opcode_t opcode = (opcode_t)raw_opcode;

    switch (opcode) {
        case LUI:
        case AUIPC: {
            utype_t decoded = decodeU(instruction);
            executeU(cpu, mem, decoded);
            break;
        }
        
        case JAL: {
            jtype_t decoded = decodeJ(instruction);
            executeJ(cpu, mem, decoded);
            break;
        }

        case JALR:
        case LOAD:
        case OP_IMM:
        case SYSTEM:
        case MISC_MEM:
        case OP_IMM_32: {
            itype_t decoded = decodeI(instruction);
            executeI(cpu, mem, decoded);
            break;
        }

        case BRANCH: {
            btype_t decoded = decodeB(instruction);
            executeB(cpu, mem, decoded);
            break;
        }

        case STORE: {
            stype_t decoded = decodeS(instruction);
            executeS(cpu, mem, decoded);
            break;
        }

        case OP:
        case OP_32: {
            rtype_t decoded = decodeR(instruction);
            executeR(cpu, mem, decoded);
            break;
        }

        default: {
        // illegal instruction
            fprintf(stderr, "Illegal instruction\n"); 
        }
    }

    if (opcode != JAL && opcode != JALR && opcode != BRANCH)
        cpu->pc += 4;
}
