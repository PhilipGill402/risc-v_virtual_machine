#ifndef INCLUDE_INSTRUCTIONS_UTYPE_H_
#define INCLUDE_INSTRUCTIONS_UTYPE_H_

#include "instructions/decoding.h"

typedef struct cpu_t cpu_t;

void executeU(cpu_t* cpu, memory_t* mem, utype_t instruction);

#endif
