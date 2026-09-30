#ifndef INCLUDE_INSTRUCTIONS_JTYPE_H_
#define INCLUDE_INSTRUCTIONS_JTYPE_H_

#include "cpu/instructions/decoding.h"

typedef struct cpu_t cpu_t;

void executeJ(cpu_t* cpu, jtype_t instruction);



#endif
