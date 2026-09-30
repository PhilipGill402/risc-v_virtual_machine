#ifndef EMULATOR_INCLUDE_CSRS_UCSR_H_
#define EMULATOR_INCLUDE_CSRS_UCSR_H_

#include <stdint.h>
#include "cpu/csrs/csr_def.h"

void ucsr_load_table(cpu_t* cpu);
void ucsr_reset(cpu_t* cpu);

#endif
