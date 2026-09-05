#ifndef INCLUDE_MEMORY_H_
#define INCLUDE_MEMORY_H_

#include <stdint.h>
#include <stddef.h>

#define MEM_BASE 0x80000000
#define MEM_SIZE 64 * 1024 * 1024 

typedef struct memory {
    uint8_t* mem;

    
} memory_t;

memory_t memory_init();

void mem_write8(memory_t* mem, uint64_t address, uint8_t value);
void mem_write16(memory_t* mem, uint64_t address, uint16_t value);
void mem_write32(memory_t* mem, uint64_t address, uint32_t value);
void mem_write64(memory_t* mem, uint64_t address, uint64_t value);

uint8_t mem_read8(memory_t* mem, uint64_t address);
uint16_t mem_read16(memory_t* mem, uint64_t address);
uint32_t mem_read32(memory_t* mem, uint64_t address);
uint64_t mem_read64(memory_t* mem, uint64_t address);


#endif
