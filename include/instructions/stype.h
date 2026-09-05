#ifndef INCLUDE_INSTRUCTIONS_STYPE_H_
#define INCLUDE_INSTRUCTIONS_STYPE_H_

#include "instructions/decoding.h"
#include "memory.h"

typedef struct cpu_t cpu_t;

void executeS(cpu_t* cpu, memory_t* mem, stype_t instruction);

#endif
