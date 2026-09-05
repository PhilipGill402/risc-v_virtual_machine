#include "instructions/utype.h"
#include "cpu.h"
#include <stdio.h>

static void lui(cpu_t* cpu, memory_t* mem, utype_t instruction) {
    (void)mem;
    cpu_write_reg(cpu, instruction.rd, instruction.imm);
}

static void auipc(cpu_t* cpu, memory_t* mem, utype_t instruction) {
    (void)mem;
    cpu_write_reg(cpu, instruction.rd, cpu->pc + instruction.imm);
}

void executeU(cpu_t* cpu, memory_t* mem, utype_t instruction) {
    opcode_t opcode = (opcode_t)instruction.opcode;
    
    if (opcode == LUI)
        lui(cpu, mem, instruction);
    else if (opcode == AUIPC)
        auipc(cpu, mem, instruction);
    else 
        fprintf(stderr, "Illegal instruction\n");
    
}
