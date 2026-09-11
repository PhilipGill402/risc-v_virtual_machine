#ifndef EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_LUI_H_
#define EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_LUI_H_

#include "instructions/decoding.h"
#include "memory.h"

typedef struct cpu_t cpu_t;

void dispatch_lui(cpu_t* cpu, memory_t* mem, utype_t instruction);

#endif
