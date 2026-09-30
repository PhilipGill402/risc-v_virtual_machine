#ifndef EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_SYSTEM_H_
#define EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_SYSTEM_H_

#include "instructions/decoding.h"

typedef struct cpu_t cpu_t;

void dispatch_system(cpu_t* cpu, itype_t instruction);

#endif
