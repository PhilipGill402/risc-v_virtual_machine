#ifndef EMULATOR_INCLUDE_CSRS_SCSR_H_
#define EMULATOR_INCLUDE_CSRS_SCSR_H_

#include <stdint.h>
#include "csrs/csr_def.h"

#define SSTATUS_WRITE_MASK \
    ((1ULL << 1)  |  /* SIE  */ \
     (1ULL << 5)  |  /* SPIE */ \
     (1ULL << 8)  |  /* SPP  */ \
     (1ULL << 18) |  /* SUM  */ \
     (1ULL << 19))   /* MXR  */

#define SUPERVISOR_INTERRUPT_MASK \
    ((1ULL << 1)| /* SSI* */ \
    (1ULL << 5) | /* STI* */ \
    (1ULL << 9) | /* SEI* */ \
    (1ULL << 13)) /* LCOFI* */ \

void scsr_load_table(cpu_t* cpu);
void scsr_reset(cpu_t* cpu);

#endif
