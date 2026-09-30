#ifndef INCLUDE_INSTRUCTIONS_ATYPE_H_
#define INCLUDE_INSTRUCTIONS_ATYPE_H_

#include "instructions/decoding.h"

typedef struct cpu_t cpu_t;

void executeA(cpu_t* cpu, atype_t instruction);

#endif
