#ifndef EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_A_EXTENSION_H_
#define EMULATOR_INCLUDE_INSTRUCTIONS_DISPATCHERS_A_EXTENSION_H_

#include "instructions/decoding.h"
#include "memory.h"

#define AMOADD   0x00
#define AMOSWAP  0x01
#define LR       0x02
#define SC       0x03
#define AMOXOR   0x04
#define AMOOR    0x08
#define AMOAND   0x0C
#define AMOMIN   0x10
#define AMOMAX   0x14
#define AMOMINU  0x18
#define AMOMAXU  0x1C

typedef struct cpu_t cpu_t;

void dispatch_a_extension(cpu_t* cpu, memory_t* mem, atype_t instruction);

#endif
