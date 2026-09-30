#ifndef EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_MISC_MEM_H_
#define EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_MISC_MEM_H_

#include "cpu/instructions/decoding.h"

typedef struct cpu_t cpu_t;

void dispatch_misc_mem(cpu_t* cpu, itype_t instruction);

#endif
