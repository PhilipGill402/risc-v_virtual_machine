#ifndef INCLUDE_INSTRUCTIONS_STYPE_H_
#define INCLUDE_INSTRUCTIONS_STYPE_H_

#include "cpu/instructions/decoding.h"

typedef struct cpu_t cpu_t;

void executeS(cpu_t* cpu, stype_t instruction);

#endif
