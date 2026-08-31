#ifndef INCLUDE_MEMORY_H_
#define INCLUDE_MEMORY_H_

#include <stdint.h>
#include <stddef.h>

class Memory {
public:
    uint8_t* mem;

    size_t address_to_index(uint64_t address) const;

    static constexpr uint64_t MEM_BASE = 0x80000000;
    static constexpr size_t MEM_SIZE = 64 * 1024 * 1024; // 64 Mib

    Memory();

    void write8(uint64_t address, uint8_t value);
    void write16(uint64_t address, uint16_t value);
    void write32(uint64_t address, uint32_t value);
    void write64(uint64_t address, uint64_t value);

    uint8_t read8(uint64_t address) const;
    uint16_t read16(uint64_t address) const;
    uint32_t read32(uint64_t address) const;
    uint64_t read64(uint64_t address) const;
};

#endif
