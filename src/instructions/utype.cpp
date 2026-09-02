#include "instructions/utype.h"
#include "cpu.h"
#include <stdio.h>

static void lui(CPU& cpu, UType instruction) {
    cpu.write_reg(instruction.rd, instruction.imm);
}

static void auipc(CPU& cpu, UType instruction) {
    cpu.write_reg(instruction.rd, cpu.get_pc() + instruction.imm);
}

void executeU(CPU& cpu, UType instruction) {
    Opcode opcode = static_cast<Opcode>(instruction.opcode);
    
    if (opcode == Opcode::LUI)
        lui(cpu, instruction);
    else if (opcode == Opcode::AUIPC)
        auipc(cpu, instruction);
    else 
        fprintf(stderr, "Illegal instruction\n");
    
}
