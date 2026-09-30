#ifndef EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_OP_IMM_H_
#define EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_OP_IMM_H_

#include "instructions/decoding.h"

typedef struct cpu_t cpu_t;

void dispatch_op_imm(cpu_t* cpu, itype_t instruction);

#endif
