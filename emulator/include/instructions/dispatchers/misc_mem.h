#ifndef EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_MISC_MEM_H_
#define EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_MISC_MEM_H_

#include "instructions/decoding.h"
#include "memory.h"

typedef struct cpu_t cpu_t;

void dispatch_misc_mem(cpu_t* cpu, memory_t* mem, itype_t instruction);

#endif
