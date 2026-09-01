#ifndef INCLUDE_CPU_H_
#define INCLUDE_CPU_H_

#include <stdint.h>
#include "registers.h"
#include "memory.h"
#include "instruction_types.h"

class CPU {
    Registers regs;
    uint64_t pc;

public:
    CPU() = default;
    void reset();
    uint32_t fetch(Memory mem);
};

#endif
