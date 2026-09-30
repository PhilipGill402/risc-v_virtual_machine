#ifndef EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_BRANCH_H_
#define EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_BRANCH_H_

#include "instructions/decoding.h"

typedef struct cpu_t cpu_t;

void dispatch_branch(cpu_t* cpu, btype_t instruction);

#endif
