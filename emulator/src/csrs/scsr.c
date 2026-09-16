#include "csrs/scsr.h"
#include "cpu.h"

static void sstatus_write(cpu_t* cpu, uint64_t value) {
    uint64_t mstatus = cpu->csrs[CSR_MSTATUS];
    mstatus = (mstatus & ~SSTATUS_WRITE_MASK) | (value & SSTATUS_WRITE_MASK);
    
    cpu->csrs[CSR_MSTATUS] = mstatus;
}

static uint64_t sstatus_read(cpu_t* cpu) {
    uint64_t value = cpu->csrs[CSR_MSTATUS] & SSTATUS_WRITE_MASK;

    // UXL = 2
    value |= (2ULL << 32);

    return value
}

static void scsr_load_sstatus(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_SSTATUS];
    csr->implemented = 1;
    csr->write = sstatus_write;
    csr->read = sstatus_read;
}

static void stvec_write(cpu_t* cpu, uint64_t value) {
    value &= ~0x3; // clear bottom 2 bits as direct mode is the only supported mode currently
    cpu->csrs[CSR_STVEC] = value;
}

static void scsr_load_stvec(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_STVEC];
    csr->implemented = 1;
    csr->write = stvec_write;
}

static void scsr_load_scause(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_SCAUSE];
    csr->implemented = 1;
    csr->write_mask = UINT64_MAX; // can write whatever
}

static uint64_t sepc_read(cpu_t* cpu) {
    return cpu->csrs[CSR_SEPC] & ~0x3; // IALIGN == 32 so clear bottom two bits
}

static void scsr_load_sepc(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_SEPC];
    csr->implemented = 1;
    csr->read = sepc_read;
    csr->write_mask = UINT64_MAX;
}

static void scsr_load_stval(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_STVAL];
    csr->implemented = 1;
    csr->write_mask = UINT64_MAX;
}

static void scsr_load_sscratch(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_SSCRATCH];
    csr->implemented = 1;
    csr->write_mask = UINT64_MAX;
}

static uint64_t sie_read(cpu_t* cpu) {
    uint64_t mie = cpu->csrs[CSR_MIE];
    uint64_t mideleg = cpu->csrs[CSR_MIDELEG];

    return mie & mideleg & SUPERVISOR_INTERRUPT_MASK;
}

static void sie_write(cpu_t* cpu, uint64_t value) {
    uint64_t mie = cpu->csrs[CSR_MIE];
    uint64_t mideleg = cpu->csrs[CSR_MIDELEG];
    uint64_t mask = SUPERVISOR_INTERRUPT_MASK & mideleg;

    mie = (mie & ~mask) | (value & mask);
    
    cpu->csrs[CSR_MIE] = mie;
}

static void scsr_load_sie(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_SIE];
    csr->implemented = 1;
    csr->write = sie_write;
    csr->read = sie_read;
}

static uint64_t sip_read(cpu_t* cpu) {
    uint64_t mip = cpu->csrs[CSR_MIP];
    uint64_t mideleg = cpu->csrs[CSR_MIDELEG];

    return mip & mideleg & SUPERVISOR_INTERRUPT_MASK;
}

static void sip_write(cpu_t* cpu, uint64_t value) {
    uint64_t mip = cpu->csrs[CSR_MIP];
    uint64_t mideleg = cpu->csrs[CSR_MIDELEG];
    uint64_t mask = SUPERVISOR_INTERRUPT_MASK & mideleg;

    mip = (mip & ~mask) | (value & mask);
    
    cpu->csrs[CSR_MIP] = mip;
}

static void scsr_load_sip(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_SIP];
    csr->implemented = 1;
    csr->write = sip_write;
    csr->read = sip_read;
}

static void scsr_load_satp(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_SATP];
    csr->implemented = 1;
    csr->write_mask = 0;
}

void scsr_load_table(cpu_t* cpu) {
    scsr_load_sstatus(cpu);
    scsr_load_stvec(cpu);
    scsr_load_scause(cpu);
    scsr_load_sepc(cpu);
    scsr_load_stval(cpu);
    scsr_load_sscratch(cpu);
    scsr_load_sie(cpu);
    scsr_load_sip(cpu);
    scsr_load_satp(cpu);
}

void scsr_reset(cpu_t* cpu) {
    cpu->csrs[CSR_STVEC] = 0;
    cpu->csrs[CSR_SSCRATCH] = 0;
    cpu->csrs[CSR_SEPC] = 0;
    cpu->csrs[CSR_SCAUSE] = 0;
    cpu->csrs[CSR_STVAL] = 0;
    cpu->csrs[CSR_SATP] = 0;
}
