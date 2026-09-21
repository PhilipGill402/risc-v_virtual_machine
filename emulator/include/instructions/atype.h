#ifndef INCLUDE_INSTRUCTIONS_ATYPE_H_
#define INCLUDE_INSTRUCTIONS_ATYPE_H_

#include "instructions/decoding.h"
#include "memory.h"

typedef struct cpu_t cpu_t;

void executeA(cpu_t* cpu, memory_t* mem, atype_t instruction);

#endif
