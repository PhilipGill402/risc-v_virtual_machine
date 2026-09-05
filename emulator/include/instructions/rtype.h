#ifndef INCLUDE_INSTRUCTIONS_RTYPE_H_
#define INCLUDE_INSTRUCTIONS_RTYPE_H_

#include "instructions/decoding.h"
#include "memory.h"

typedef struct cpu_t cpu_t;

void executeR(cpu_t* cpu, memory_t* mem, rtype_t instruction);

#endif
