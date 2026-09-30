#ifndef EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_LUI_H_
#define EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_LUI_H_

#include "cpu/instructions/decoding.h"

typedef struct cpu_t cpu_t;

void dispatch_lui(cpu_t* cpu, utype_t instruction);

#endif
