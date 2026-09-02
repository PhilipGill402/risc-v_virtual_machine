#ifndef INCLUDE_REGISTERS_H_
#define INCLUDE_REGISTERS_H_

#include <stdint.h>
#include <cstddef>

class Registers {
    uint64_t regs[32]{};

public:
    Registers() = default;
    
    void write(uint8_t index, uint64_t value);
    uint64_t read(uint8_t index) const;
};

#endif
