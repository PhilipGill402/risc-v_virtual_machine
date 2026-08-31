#include "cpu.h"

void CPU::reset() {
    pc = Memory::MEM_BASE;
}

uint32_t CPU::fetch(Memory mem) {
    return mem.read32(pc);
}


