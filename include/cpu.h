#ifndef INCLUDE_CPU_H_
#define INCLUDE_CPU_H_

#include <stdint.h>
#include "registers.h"
#include "memory.h"
#include "instructions/decoding.h"
#include "instructions/itype.h"

class CPU {
    Registers regs;
    uint64_t pc;

public:
    CPU() = default;
    void reset();
    uint32_t fetch(const Memory& mem);
    void step(const Memory& mem);
    void write_reg(uint8_t reg_num, uint64_t value);
    uint64_t read_reg(uint8_t reg_num) const;
    uint64_t get_pc() const;
};

#endif
