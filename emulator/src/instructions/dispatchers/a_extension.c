#include "instructions/dispatchers/a_extension.h"
#include "cpu.h"
#include "trap.h"

static void amo_w(cpu_t* cpu, memory_t* mem, atype_t instruction);

static void amo_d(cpu_t* cpu, memory_t* mem, atype_t instruction) {
    
}

static void lr(cpu_t* cpu, memory_t* mem, atype_t instruction);
static void sc(cpu_t* cpu, memory_t* mem, atype_t instruction);

void dispatch_a_extension(cpu_t* cpu, memory_t* mem, atype_t instruction) {
    switch (instruction.funct5) {
        case AMOADD:
        case AMOSWAP:
        case AMOXOR:
        case AMOOR:
        case AMOAND:
        case AMOMIN:
        case AMOMAX:
        case AMOMINU:
        case AMOMAXU: {
            if (instruction.funct3 == 0x2)
                amo_w(cpu, mem, instruction);
            else if (instruction.funct3 == 0x3)
                amo_d(cpu, mem, instruction);
            else
                raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
            break;
        }
        case LR: lr(cpu, mem, instruction); break; 
        case SC: sc(cpu, mem, instruction); break;
        default: raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
    }
}
