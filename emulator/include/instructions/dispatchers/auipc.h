#ifndef EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_AUIPC_H_
#define EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_AUIPC_H_

#include "instructions/decoding.h"
#include "memory.h"

typedef struct cpu_t cpu_t;

void dispatch_auipc(cpu_t* cpu, memory_t* mem, utype_t instruction);

#endif
