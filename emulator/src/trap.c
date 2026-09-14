#include "trap.h"
#include "cpu.h"
#include "csrs/csr_def.h"

#include "log.h"

static void enter_trap(cpu_t* cpu) {
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

void raise_exception(cpu_t* cpu, uint64_t cause, uint64_t tvalue) {
    log_debug("Exception raised\n");

    cpu->csrs[CSR_MEPC] = cpu->pc;
    cpu->csrs[CSR_MCAUSE] = cause;
    cpu->csrs[CSR_MTVAL] = tvalue;
    
    enter_trap(cpu); 
}

void raise_interrupt(cpu_t* cpu, uint64_t cause) {
    cpu->csrs[CSR_MEPC] = cpu->pc;
    cpu->csrs[CSR_MCAUSE] = (1ULL << 63) | cause; // set interrupt bit
    cpu->csrs[CSR_MTVAL] = 0;

    enter_trap(cpu); 
}
