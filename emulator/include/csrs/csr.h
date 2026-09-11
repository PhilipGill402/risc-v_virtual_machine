#ifndef EMULATOR_INCLUDE_CSRS_CSR_H_
#define EMULATOR_INCLUDE_CSRS_CSR_H_

#include <stdint.h>

#define CSR_READ_ONLY   0b11
#define U_MODE          0b00
#define S_MODE          0b01
#define M_MODE          0b11

typedef uint8_t priviledge_t;
typedef struct cpu_t cpu_t;

typedef enum {
    CSR_OK,
    CSR_ILLEGAL
} csr_status_t;


csr_status_t csr_read(cpu_t *cpu, uint16_t address, uint64_t* value);
csr_status_t csr_write(cpu_t *cpu, uint16_t address, uint64_t value);
void csr_load(cpu_t* cpu);
void csr_reset(cpu_t* cpu);

#endif
