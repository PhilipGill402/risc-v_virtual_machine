#ifndef EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_STORE_H_
#define EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_STORE_H_

#include "instructions/decoding.h"
#include "memory.h"

typedef struct cpu_t cpu_t;

void dispatch_store(cpu_t* cpu, memory_t* mem, stype_t instruction);

#endif
