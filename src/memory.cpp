#include "memory.h"
#include <sys/mman.h>
#include <errno.h>
#include <stdio.h>
#include <stdexcept>

Memory::Memory() {
    errno = 0;
    mem = reinterpret_cast<uint8_t*>(mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    if (mem == MAP_FAILED) {
        perror("mmap");
        exit(0);
    }
}

size_t Memory::address_to_index(uint64_t address) const {
    if (address < MEM_BASE || address >= MEM_BASE + MEM_SIZE)
        throw std::out_of_range("Address out of memory range");

    return static_cast<size_t>(address - MEM_BASE);
}

void Memory::write8(uint64_t address, uint8_t value) {
    size_t idx = address_to_index(address);

    mem[idx] = value;
}

void Memory::write16(uint64_t address, uint16_t value) {}
void Memory::write32(uint64_t address, uint32_t value) {}
void Memory::write64(uint64_t address, uint64_t value) {}

uint8_t Memory::read8(uint64_t address) const {
    size_t idx = address_to_index(address);

    return mem[idx];
}

uint16_t Memory::read16(uint64_t address) const {}
uint32_t Memory::read32(uint64_t address) const {}
uint64_t Memory::read64(uint64_t address) const {}
