#ifndef EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_OP_H_
#define EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_OP_H_

#include "cpu/instructions/decoding.h"

typedef struct cpu_t cpu_t;

void dispatch_op(cpu_t* cpu, rtype_t instruction);

#endif
