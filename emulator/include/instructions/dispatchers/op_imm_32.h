#ifndef EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_OP_IMM_32_H_
#define EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_OP_IMM_32_H_

#include "instructions/decoding.h"
#include "memory.h"

typedef struct cpu_t cpu_t;

void dispatch_op_imm_32(cpu_t* cpu, memory_t* mem, itype_t instruction);

#endif
