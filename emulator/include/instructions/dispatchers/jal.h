#ifndef EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_JAL_H_
#define EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_JAL_H_

#include "instructions/decoding.h"

typedef struct cpu_t cpu_t;

void dispatch_jal(cpu_t* cpu, jtype_t instruction);

#endif
