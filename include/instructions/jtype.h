#ifndef INCLUDE_INSTRUCTIONS_JTYPE_H_
#define INCLUDE_INSTRUCTIONS_JTYPE_H_

#include "instructions/decoding.h"
#include "memory.h"

typedef struct cpu_t cpu_t;

void executeJ(cpu_t* cpu, memory_t* mem, jtype_t instruction);



#endif
