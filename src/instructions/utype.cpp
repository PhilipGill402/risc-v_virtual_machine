#include "instructions/utype.h"
#include "cpu.h"
#include <stdio.h>

static void lui(CPU& cpu, Memory& mem, UType instruction) {
    (void)mem;
    cpu.write_reg(instruction.rd, instruction.imm);
}

static void auipc(CPU& cpu, Memory& mem, UType instruction) {
    (void)mem;
    cpu.write_reg(instruction.rd, cpu.pc + instruction.imm);
}

void executeU(CPU& cpu, Memory& mem, UType instruction) {
    Opcode opcode = static_cast<Opcode>(instruction.opcode);
    
    if (opcode == Opcode::LUI)
        lui(cpu, mem, instruction);
    else if (opcode == Opcode::AUIPC)
        auipc(cpu, mem, instruction);
    else 
        fprintf(stderr, "Illegal instruction\n");
    
}
