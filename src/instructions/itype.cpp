#include "instructions/itype.h"
#include "cpu.h"

#include <stdio.h>

static void addi(CPU& cpu, IType instruction) {
    uint64_t reg_value = cpu.read_reg(instruction.rs1);

    cpu.write_reg(instruction.rd, reg_value + instruction.imm);
}

void executeI(CPU& cpu, IType instruction) {
    switch (instruction.funct3) {
        case 0x0: {
            addi(cpu, instruction);
        } 
    }
}
