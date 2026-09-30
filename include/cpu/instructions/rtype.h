#ifndef INCLUDE_INSTRUCTIONS_RTYPE_H_
#define INCLUDE_INSTRUCTIONS_RTYPE_H_

#include "cpu/instructions/decoding.h"

typedef struct cpu_t cpu_t;

void executeR(cpu_t* cpu, rtype_t instruction);

#endif
