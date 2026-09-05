#ifndef INCLUDE_INSTRUCTIONS_ITYPE_H_
#define INCLUDE_INSTRUCTIONS_ITYPE_H_

#include "instructions/decoding.h"
#include "memory.h"

typedef struct cpu_t cpu_t;

void executeI(cpu_t* cpu, memory_t* mem, itype_t instruction);

#endif
