#include "csrs/mcsr.h"
#include "cpu.h"

#include <stdio.h>

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

static void mcsr_load_mstatus(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_MSTATUS];
    csr->implemented = 1;
    csr->write_mask = (1ULL << 1)  | // SIE
                    (1ULL << 3)  | // MIE
                    (1ULL << 5)  | // SPIE
                    (1ULL << 7)  | // MPIE
                    (1ULL << 8)  | // SPP
                    (3ULL << 11) | // MPP
                    (1ULL << 17) | // MPRV
                    (1ULL << 18) | // SUM
                    (1ULL << 19) | // MXR
                    (1ULL << 20);  // TVM;
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
    csr->write_mask = UINT64_MAX;
}

static void mcsr_load_mideleg(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_MIDELEG];
    csr->implemented = 1;
    csr->write_mask = UINT64_MAX;
}

static void mcsr_load_mip(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_MIP];
    csr->implemented = 1;
    csr->write_mask = 0;
}

static void mcsr_load_mie(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_MIE];
    csr->implemented = 1;
    csr->write_mask = (1ULL << 1) |
                    (1ULL << 3) |
                    (1ULL << 5) |
                    (1ULL << 7) |
                    (1ULL << 9) |
                    (1ULL << 11);
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

static void mcsr_load_mhartid(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_MHARTID];
    csr->implemented = 1;
    csr->write_mask = 0; // read only
}

static void mcsr_load_mvendorid(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_MVENDORID];
    csr->implemented = 1;
    csr->write_mask = 0; // read only
}

static void mcsr_load_marchid(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_MARCHID];
    csr->implemented = 1;
    csr->write_mask = 0; // read only
}

static void mcsr_load_mimpid(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_MIMPID];
    csr->implemented = 1;
    csr->write_mask = 0; // read only
}

static void mcsr_load_mcounteren(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_MCOUNTEREN];
    csr->implemented = 1;
    csr->write_mask = 0x7;
}

static void mcsr_load_mcycle(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_MCYCLE];
    csr->implemented = 1;
    csr->write_mask = UINT64_MAX;
}

static void mcsr_load_minstret(cpu_t* cpu) {
    csr_descriptor_t* csr = &csr_table[CSR_MINSTRET];
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
    mcsr_load_mhartid(cpu);
    mcsr_load_mvendorid(cpu);
    mcsr_load_marchid(cpu);
    mcsr_load_mimpid(cpu);
    mcsr_load_mcounteren(cpu);
    mcsr_load_mcycle(cpu);
    mcsr_load_minstret(cpu);
}

void mcsr_reset(cpu_t* cpu) {
    cpu->csrs[CSR_MISA] = MISA_MXL_RV64 | MISA_A | MISA_I | MISA_M | MISA_S | MISA_U;
    cpu->csrs[CSR_MSTATUS] = 3ULL << 11; // MIE = 0, MPP = M
    cpu->csrs[CSR_MTVEC] = MEM_BASE;
    cpu->csrs[CSR_MEDELEG] = 0;
    cpu->csrs[CSR_MIDELEG] = 0;
    cpu->csrs[CSR_MIP] = 0;
    cpu->csrs[CSR_MIE] = 0;
    cpu->csrs[CSR_MSCRATCH] = 0;
    cpu->csrs[CSR_MEPC] = 0;
    cpu->csrs[CSR_MCAUSE] = 0;
    cpu->csrs[CSR_MTVAL] = 0;
    cpu->csrs[CSR_MHARTID] = 0;
    cpu->csrs[CSR_MVENDORID] = 0;
    cpu->csrs[CSR_MARCHID] = 0;
    cpu->csrs[CSR_MIMPID] = 0;
    cpu->csrs[CSR_MCOUNTEREN] = 0;
    cpu->csrs[CSR_MCYCLE] = 0;
    cpu->csrs[CSR_MINSTRET] = 0;
}
