#ifndef EMULATOR_INCLUDE_CSRS_CSR_NAMES_H_
#define EMULATOR_INCLUDE_CSRS_CSR_NAMES_H_

#include <stdint.h>

#define CSR_MSTATUS   0x300
#define CSR_MISA      0x301
#define CSR_MEDELEG   0x302
#define CSR_MIDELEG   0x303
#define CSR_MIE       0x304
#define CSR_MTVEC     0x305
#define CSR_MSCRATCH  0x340
#define CSR_MEPC      0x341
#define CSR_MCAUSE    0x342
#define CSR_MTVAL     0x343
#define CSR_MIP       0x344

#define CSR_SSTATUS   0x100
#define CSR_SIE       0x104
#define CSR_STVEC     0x105
#define CSR_SSCRATCH  0x140
#define CSR_SEPC      0x141
#define CSR_SCAUSE    0x142
#define CSR_STVAL     0x143
#define CSR_SIP       0x144
#define CSR_SATP      0x180

typedef struct cpu_t cpu_t;

typedef struct {
    uint8_t implemented;
    uint64_t write_mask;
    uint64_t (*read)(cpu_t *cpu);
    void (*write)(cpu_t *cpu, uint64_t value);
} csr_descriptor_t;

extern csr_descriptor_t csr_table[4096];

#endif
