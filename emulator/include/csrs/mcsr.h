#ifndef EMULATOR_INCLUDE_CSRS_MCSR_H_
#define EMULATOR_INCLUDE_CSRS_MCSR_H_

#include <stdint.h>
#include "csrs/csr_def.h"

#define MTVEC_DIRECT_MODE   0x0
#define MTVEC_VECTORED_MODE 0x1

void mcsr_load_table(cpu_t* cpu);
void mcsr_reset(cpu_t* cpu);

#endif
