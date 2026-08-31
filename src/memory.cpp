#include "memory.h"
#include <sys/mman.h>
#include <errno.h>
#include <stdio.h>
#include <stdexcept>

#include <stdio.h>

Memory::Memory() {
    errno = 0;
    mem = reinterpret_cast<uint8_t*>(mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    if (mem == MAP_FAILED) {
        perror("mmap");
        exit(0);
    }
}

bool Memory::valid_access(uint64_t address, size_t size) const {
    if (address < MEM_BASE)
        return false;

    uint64_t offset = address - MEM_BASE;

    return offset <= MEM_SIZE - size;
}

size_t Memory::address_to_index(uint64_t address) const {
    return static_cast<size_t>(address - MEM_BASE);
}

void Memory::write8(uint64_t address, uint8_t value) {
    if (!valid_access(address, 1))
        throw std::out_of_range("Memory address out of bounds");

    size_t idx = address_to_index(address);

    mem[idx] = value;
}

void Memory::write16(uint64_t address, uint16_t value) {
    if (!valid_access(address, 2))
        throw std::out_of_range("Memory address out of bounds");

    size_t idx = address_to_index(address);
    
    mem[idx] = static_cast<uint8_t>(value >> 8);
    mem[idx + 1] = static_cast<uint8_t>(value & 0x00FF);
}

void Memory::write32(uint64_t address, uint32_t value) {
    if (!valid_access(address, 4))
        throw std::out_of_range("Memory address out of bounds");

    size_t idx = address_to_index(address);

    mem[idx] = static_cast<uint8_t>(value >> 24);
    mem[idx + 1] = static_cast<uint8_t>((value & 0x00FF0000) >> 16);
    mem[idx + 2] = static_cast<uint8_t>((value & 0x0000FF00) >> 8);
    mem[idx + 3] = static_cast<uint8_t>(value & 0x000000FF);
}

void Memory::write64(uint64_t address, uint64_t value) {
    if (!valid_access(address, 8))
        throw std::out_of_range("Memory address out of bounds");
    
    size_t idx = address_to_index(address);

    mem[idx] = static_cast<uint8_t>(value >> 56);
    mem[idx + 1] = static_cast<uint8_t>((value & 0x00FF000000000000) >> 48);
    mem[idx + 2] = static_cast<uint8_t>((value & 0x0000FF0000000000) >> 40);
    mem[idx + 3] = static_cast<uint8_t>((value & 0x000000FF00000000) >> 32);
    mem[idx + 4] = static_cast<uint8_t>((value & 0x00000000FF000000) >> 24);
    mem[idx + 5] = static_cast<uint8_t>((value & 0x0000000000FF0000) >> 16);
    mem[idx + 6] = static_cast<uint8_t>((value & 0x000000000000FF00) >> 8);
    mem[idx + 7] = static_cast<uint8_t>(value & 0x000000000000000FF); 
}

uint8_t Memory::read8(uint64_t address) const {
    if (!valid_access(address, 1))
        throw std::out_of_range("Memory address out of bounds");
    
    size_t idx = address_to_index(address);

    return mem[idx];
}

uint16_t Memory::read16(uint64_t address) const {
    if (!valid_access(address, 2))
        throw std::out_of_range("Memory address out of bounds");

    size_t idx = address_to_index(address);

    return static_cast<uint16_t>((mem[idx] << 8) | mem[idx + 1]);
}

uint32_t Memory::read32(uint64_t address) const {
    if (!valid_access(address, 4))
        throw std::out_of_range("Memory address out of bounds");

    size_t idx = address_to_index(address);

    return static_cast<uint32_t>((mem[idx] << 24) | (mem[idx + 1] << 16) | (mem[idx + 2] << 8) | (mem[idx + 3]));
}

uint64_t Memory::read64(uint64_t address) const {
    if (!valid_access(address, 8))
        throw std::out_of_range("Memory address out of bounds");

    size_t idx = address_to_index(address);

    return static_cast<uint64_t>((static_cast<uint64_t>(mem[idx]) << 56) | (static_cast<uint64_t>(mem[idx + 1]) << 48) | (static_cast<uint64_t>(mem[idx + 2]) << 40) | (static_cast<uint64_t>(mem[idx + 3]) << 32) | (mem[idx + 4] << 24) | (mem[idx + 5] << 16) | (mem[idx + 6] << 8) | (mem[idx]));
}


