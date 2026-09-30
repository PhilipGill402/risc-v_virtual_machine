#ifndef EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_AUIPC_H_
#define EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_AUIPC_H_

#include "instructions/decoding.h"

typedef struct cpu_t cpu_t;

void dispatch_auipc(cpu_t* cpu, utype_t instruction);

#endif
