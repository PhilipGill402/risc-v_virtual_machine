#ifndef EMULATOR_INCLUDE_CSRS_MCSR_H_
#define EMULATOR_INCLUDE_CSRS_MCSR_H_

#include <stdint.h>
#include "csrs/csr_def.h"

#define MTVEC_DIRECT_MODE   0x0
#define MTVEC_VECTORED_MODE 0x1

#define MISA_A (1ULL << 0)
#define MISA_I (1ULL << 8)
#define MISA_M (1ULL << 12)
#define MISA_S (1ULL << 18)
#define MISA_U (1ULL << 20)

#define MISA_MXL_RV64 (2ULL << 62)

void mcsr_load_table(cpu_t* cpu);
void mcsr_reset(cpu_t* cpu);

#endif
