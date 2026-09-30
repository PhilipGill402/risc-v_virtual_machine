#ifndef INCLUDE_INSTRUCTIONS_ITYPE_H_
#define INCLUDE_INSTRUCTIONS_ITYPE_H_

#include "cpu/instructions/decoding.h"

typedef struct cpu_t cpu_t;

void executeI(cpu_t* cpu, itype_t instruction);

#endif
