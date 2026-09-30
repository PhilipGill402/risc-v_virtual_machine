#ifndef EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_LOAD_H_
#define EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_LOAD_H_

#include "cpu/instructions/decoding.h"

typedef struct cpu_t cpu_t;

void dispatch_load(cpu_t* cpu, itype_t instruction);

#endif
