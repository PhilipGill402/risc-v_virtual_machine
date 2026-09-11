#ifndef EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_SYSTEM_H_
#define EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_SYSTEM_H_

#include "instructions/decoding.h"
#include "memory.h"

typedef struct cpu_t cpu_t;

void dispatch_system(cpu_t* cpu, memory_t* mem, itype_t instruction);

#endif
