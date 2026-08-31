#include "registers.h"
#include <stdexcept>

void Registers::write(uint8_t index, uint64_t value) {
    if (index == 0)
        return;
    
    if (index >= 32)
        throw std::out_of_range("Register number out of range");

    this->regs[index] = value;
}

uint64_t Registers::read(uint8_t index) const {
    if (index >= 32)
        throw std::out_of_range("Register number out of range");

    return this->regs[index];
}
