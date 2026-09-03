#include "instructions/rtype.h"
#include "cpu.h"

#include <stdio.h>

static void add(CPU& cpu, Memory& mem, RType instruction) {
    uint64_t op1 = cpu.read_reg(instruction.rs1);
    uint64_t op2 = cpu.read_reg(instruction.rs2);

    cpu.write_reg(instruction.rd, op1 + op2);
}

static void sub(CPU& cpu, Memory& mem, RType instruction) {
    uint64_t op1 = cpu.read_reg(instruction.rs1);
    uint64_t op2 = cpu.read_reg(instruction.rs2);

    cpu.write_reg(instruction.rd, op1 - op2);
}

static void sll(CPU& cpu, Memory& mem, RType instruction) {

}

static void slt(CPU& cpu, Memory& mem, RType instruction) {

}

static void sltu(CPU& cpu, Memory& mem, RType instruction) {

}

static void xor_op(CPU& cpu, Memory& mem, RType instruction) {

}

static void srl(CPU& cpu, Memory& mem, RType instruction) {

}

static void or_op(CPU& cpu, Memory& mem, RType instruction) {

}

static void and_op(CPU& cpu, Memory& mem, RType instruction) {

}

static void dispatch_op(CPU& cpu, Memory& mem, RType instruction) {
    switch (instruction.funct3) {
        case 0x0: {
            if (instruction.funct7 == 0b0000000)
                add(cpu, mem, instruction);
            else if (instruction.funct7 == 0b0100000)
                sub(cpu, mem, instruction);
            break;
        }
        case 0x1: sll(cpu, mem, instruction); break;
        case 0x2: slt(cpu, mem, instruction); break;
        case 0x3: sltu(cpu, mem, instruction); break;
        case 0x4: xor_op(cpu, mem, instruction); break;
        case 0x5: srl(cpu, mem, instruction); break;
        case 0x6: or_op(cpu, mem, instruction); break;
        case 0x7: and_op(cpu, mem, instruction); break;
        default: fprintf(stderr, "Illegal instruction\n");
    }
}

static void dispatch_op_32(CPU& cpu, Memory& mem, RType instruction) {

}

void executeR(CPU& cpu, Memory& mem, RType instruction) {
    Opcode opcode = static_cast<Opcode>(instruction.opcode);
    
    if (opcode == Opcode::OP)
        dispatch_op(cpu, mem, instruction);
    else if (opcode == Opcode::OP_32)
        dispatch_op_32(cpu, mem, instruction);
    else
        fprintf(stderr, "Illegal instruction\n");
}



