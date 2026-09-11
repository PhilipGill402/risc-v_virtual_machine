#include "instructions/dispatchers/system.h"
#include "cpu.h"
#include "exception.h"
#include "log.h"

static void ecall(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    uint64_t cause = EXC_ECALL_U_MODE; 
    if (cpu->priviledge == M_MODE)
        cause = EXC_ECALL_M_MODE;
    else if (cpu->priviledge == S_MODE)
        cause = EXC_ECALL_S_MODE;
    else if (cpu->priviledge == U_MODE)
        cause = EXC_ECALL_U_MODE;

    raise_exception(cpu, cause, 0);
    printf("%llx\n", cpu->pc);
}

static void ebreak(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    raise_exception(cpu, EXC_BREAKPOINT, 0);    
}

static void sret(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    return;
}

static void mret(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    return;
}

static void wfi(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    return;
}

void dispatch_system(cpu_t* cpu, memory_t* mem, itype_t instruction) {
    if (instruction.rs1 != 0 || instruction.rd != 0 || instruction.funct3 != 0) {
        raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
        return;
    } 

    switch (instruction.imm) {
        case 0x000: ecall(cpu, mem, instruction); break;
        case 0x001: ebreak(cpu, mem, instruction); break;
        case 0x102: sret(cpu, mem, instruction); break;
        case 0x302: mret(cpu, mem, instruction); break;
        case 0x105: wfi(cpu, mem, instruction); break;
        default: raise_exception(cpu, EXC_ILLEGAL_INSTRUCTION, 0);
    }
}
