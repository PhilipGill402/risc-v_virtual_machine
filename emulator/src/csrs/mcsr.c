#include "csrs/mcsr.h"
#include "cpu.h"

static void misa_write(cpu_t* cpu, uint64_t value) {
    (void)cpu;
    (void)value;
    //ignore all writes for now
    return;
}

static void mcsr_load_misa(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_MISA];

    csr->implemented = 1;
    csr->write = misa_write;
}

static void mstatus_write(cpu_t* cpu, uint64_t value) {
    uint8_t mie = (uint8_t)(value >> 3) & 0x1;
    uint8_t mpie = (uint8_t)(value >> 7) & 0x1;
    uint8_t mpp = (uint8_t)(value >> 11) & 0x3;

    uint64_t new_value = cpu->csrs[CSR_MSTATUS];
    new_value = set_bit(new_value, 3, mie);
    new_value = set_bit(new_value, 7, mpie);

    if (mpp == 3) {
        new_value = set_bit(new_value, 11, 1);
        new_value = set_bit(new_value, 12, 1);
    }

    cpu->csrs[CSR_MSTATUS] = new_value;
}

static void mcsr_load_mstatus(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_MSTATUS];
    csr->implemented = 1;
    csr->write = mstatus_write;
}

static void mtvec_write(cpu_t* cpu, uint64_t value) {
    value &= ~0x3ULL; // we only support direct mode currently

    cpu->csrs[CSR_MTVEC] = value;
}

static void mcsr_load_mtvec(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_MTVEC];
    csr->implemented = 1;
    csr->write = mtvec_write;
}

static void mcsr_load_medeleg(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_MEDELEG];
    csr->implemented = 1;
    csr->write_mask = 0; // disable writes as no other priviledge levels exist
}

static void mcsr_load_mideleg(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_MIDELEG];
    csr->implemented = 1;
    csr->write_mask = 0; // disable writes as no other priviledge levels exist
}

static void mcsr_load_mip(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_MIP];
    csr->implemented = 1;
    csr->write_mask = 0;
}

static void mcsr_load_mie(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_MIE];
    csr->implemented = 1;
    csr->write_mask = (1ULL << 3) | (1ULL << 7) | (1ULL << 11); // only allow meie, mtie, and msie to be changed
}

static void mcsr_load_mscratch(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_MSCRATCH];
    csr->implemented = 1;
    csr->write_mask = UINT64_MAX;
}

static void mcsr_load_mepc(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_MEPC];
    csr->implemented = 1;
    csr->write_mask = ~0x3ULL;
}

static void mcsr_load_mcause(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_MCAUSE];
    csr->implemented = 1;
    csr->write_mask = UINT64_MAX;
}

static void mcsr_load_mtval(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_MTVAL];
    csr->implemented = 1;
    csr->write_mask = UINT64_MAX;
}

void mcsr_load_table(cpu_t* cpu) {
    mcsr_load_misa(cpu);
    mcsr_load_mstatus(cpu);
    mcsr_load_mtvec(cpu);
    mcsr_load_medeleg(cpu);
    mcsr_load_mideleg(cpu);
    mcsr_load_mip(cpu);
    mcsr_load_mie(cpu);
    mcsr_load_mscratch(cpu);
    mcsr_load_mepc(cpu);
    mcsr_load_mcause(cpu);
    mcsr_load_mtval(cpu);
}

void mcsr_reset(cpu_t* cpu) {
    cpu->csrs[CSR_MISA] = (2ULL << 62) | (1ULL << 8) | (1ULL << 12);
    cpu->csrs[CSR_MSTATUS] = 3ULL << 11; // MIE = 0, MPP = M
    cpu->csrs[CSR_MTVEC] = 0; // MIE = 0, MPP = M
    cpu->csrs[CSR_MEDELEG] = 0;
    cpu->csrs[CSR_MIDELEG] = 0;
    cpu->csrs[CSR_MIP] = 0;
    cpu->csrs[CSR_MIE] = 0;
    cpu->csrs[CSR_MSCRATCH] = 0;
    cpu->csrs[CSR_MEPC] = 0;
    cpu->csrs[CSR_MCAUSE] = 0;
    cpu->csrs[CSR_MTVAL] = 0;
}
