#ifndef EMULATOR_INCLUDE_INSTRUCTIONS_M_EXTENSION_H_
#define EMULATOR_INCLUDE_INSTRUCTIONS_M_EXTENSION_H_

#include "instructions/decoding.h"
#include "memory.h"

typedef struct cpu_t cpu_t;

void dispatch_m_op(cpu_t* cpu, memory_t* mem, rtype_t instruction);
void dispatch_m_op_32(cpu_t* cpu, memory_t* mem, rtype_t instruction);

#endif
