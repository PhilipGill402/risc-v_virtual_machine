#ifndef INCLUDE_INSTRUCTIONS_BTYPE_H_
#define INCLUDE_INSTRUCTIONS_BTYPE_H_

#include "cpu/instructions/decoding.h"

typedef struct cpu_t cpu_t;

void executeB(cpu_t* cpu, btype_t instruction);

#endif
