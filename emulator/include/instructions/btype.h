#ifndef INCLUDE_INSTRUCTIONS_BTYPE_H_
#define INCLUDE_INSTRUCTIONS_BTYPE_H_

#include "instructions/decoding.h"
#include "memory.h"

typedef struct cpu_t cpu_t;

void executeB(cpu_t* cpu, memory_t* mem, btype_t instruction);

#endif
