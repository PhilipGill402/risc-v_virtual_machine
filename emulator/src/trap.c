#include "trap.h"
#include "cpu.h"
#include "csrs/csr_def.h"

#include "log.h"

static void enter_m_trap(cpu_t* cpu) {
    uint64_t mstatus = cpu->csrs[CSR_MSTATUS];
    uint8_t mie = (uint8_t)(mstatus >> 3) & 0x1;

    mstatus = set_bit(mstatus, 7, mie); // MPIE = MIE
    mstatus = set_bit(mstatus, 3, 0); // MIE = 0
    
    // MPP = current priviledge 
    uint8_t high_bit = (cpu->priviledge >> 1) & 0x1;
    uint8_t low_bit = cpu->priviledge & 0x1;
    mstatus = set_bit(mstatus, 11, low_bit);
    mstatus = set_bit(mstatus, 12, high_bit);

    cpu->csrs[CSR_MSTATUS] = mstatus;
    
    cpu->priviledge = M_MODE;

    cpu->pc = cpu->csrs[CSR_MTVEC] & ~0x3ULL; // exclude mode bytes (also guarantees 4-byte boundary)
    cpu->trap_taken = 1;
}

static void enter_s_trap(cpu_t* cpu) {
    uint64_t mstatus = cpu->csrs[CSR_MSTATUS];
    uint8_t sie = (uint8_t)(mstatus >> 1) & 0x1;
    uint8_t spp = cpu->priviledge & 0x1;

    mstatus = set_bit(mstatus, 8, spp); // SPP = current priviledge
    mstatus = set_bit(mstatus, 5, sie); // SPIE = SIE
    mstatus = set_bit(mstatus, 1, 0); // SIE = 0
    cpu->csrs[CSR_MSTATUS] = mstatus; 

    cpu->priviledge = S_MODE;
    cpu->pc = cpu->csrs[CSR_STVEC] & ~0x3ULL; // exclude mode bytes (also guarantees 4-byte boundary)
    cpu->trap_taken = 1;
}

void raise_exception(cpu_t* cpu, uint64_t cause, uint64_t tvalue) {
    log_debug("Exception raised\n");
    
    uint8_t current_priviledge = cpu->priviledge;
    uint8_t medeleg_set = (cpu->csrs[CSR_MEDELEG] & (1ULL << cause)) != 0;

    if (current_priviledge < M_MODE && medeleg_set) {
        cpu->csrs[CSR_SEPC] = cpu->pc;
        cpu->csrs[CSR_SCAUSE] = cause;
        cpu->csrs[CSR_STVAL] = tvalue;

        enter_s_trap(cpu);
    } else {
        cpu->csrs[CSR_MEPC] = cpu->pc;
        cpu->csrs[CSR_MCAUSE] = cause;
        cpu->csrs[CSR_MTVAL] = tvalue;
        
        enter_m_trap(cpu);
    }
}

void raise_interrupt(cpu_t* cpu, uint64_t cause) {
    uint8_t current_priviledge = cpu->priviledge;
    uint8_t mideleg_set = (cpu->csrs[CSR_MIDELEG] & (1ULL << cause)) != 0;

    if (current_priviledge < M_MODE && mideleg_set) {
        cpu->csrs[CSR_SEPC] = cpu->pc;
        cpu->csrs[CSR_SCAUSE] = (1ULL << 63) | cause; // set interrupt bit
        cpu->csrs[CSR_STVAL] = 0;

        enter_s_trap(cpu);
    } else {
        cpu->csrs[CSR_MEPC] = cpu->pc;
        cpu->csrs[CSR_MCAUSE] = (1ULL << 63) | cause; // set interrupt bit
        cpu->csrs[CSR_MTVAL] = 0;

        enter_m_trap(cpu);
    }
}
