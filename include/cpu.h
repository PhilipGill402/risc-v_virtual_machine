#ifndef INCLUDE_CPU_H_
#define INCLUDE_CPU_H_

#include <stdint.h>
#include "registers.h"

class CPU {
    Registers regs;
    uint64_t pc;

public:
    CPU() = default;
};

#endif
