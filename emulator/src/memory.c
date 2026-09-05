#include "memory.h"
#include <sys/mman.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

memory_t memory_init() {
    memory_t mem = { 0 }; 

    errno = 0;
    mem.mem = (uint8_t*)mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem.mem == MAP_FAILED) {
        perror("mmap");
        exit(0);
    }

    return mem;
}

static uint8_t valid_access(uint64_t address, size_t size) {
    if (address < MEM_BASE)
        return 0;

    uint64_t offset = address - MEM_BASE;

    return offset <= MEM_SIZE - size;
}

static size_t address_to_index(uint64_t address) {
    return (size_t)(address - MEM_BASE);
}

void mem_write8(memory_t* mem, uint64_t address, uint8_t value) {
    if (!valid_access(address, 1)) {
        fprintf(stderr, "Invalid address: %llx\n", address);
        return;
    }
        

    size_t idx = address_to_index(address);

    mem->mem[idx] = value;
}

void mem_write16(memory_t* mem, uint64_t address, uint16_t value) {
    if (!valid_access(address, 2)) {
        fprintf(stderr, "Invalid address: %llx\n", address);
        return;
    }

    size_t idx = address_to_index(address);
    
    mem->mem[idx] = (uint8_t)(value & 0x00FF);
    mem->mem[idx + 1] = (uint8_t)(value >> 8);
}

void mem_write32(memory_t* mem, uint64_t address, uint32_t value) {
    if (!valid_access(address, 4)) {
        fprintf(stderr, "Invalid address: %llx\n", address);
        return;
    }

    size_t idx = address_to_index(address);

    mem->mem[idx] = (uint8_t)(value & 0x000000FF);
    mem->mem[idx + 1] = (uint8_t)((value & 0x0000FF00) >> 8);
    mem->mem[idx + 2] = (uint8_t)((value & 0x00FF0000) >> 16);
    mem->mem[idx + 3] = (uint8_t)(value >> 24);
}

void mem_write64(memory_t* mem, uint64_t address, uint64_t value) {
    if (!valid_access(address, 8)) {
        fprintf(stderr, "Invalid address: %llx\n", address);
        return;
    }
    
    size_t idx = address_to_index(address);

    mem->mem[idx] = (uint8_t)(value & 0x000000000000000FF); 
    mem->mem[idx + 1] = (uint8_t)((value & 0x000000000000FF00) >> 8);
    mem->mem[idx + 2] = (uint8_t)((value & 0x0000000000FF0000) >> 16);
    mem->mem[idx + 3] = (uint8_t)((value & 0x00000000FF000000) >> 24);
    mem->mem[idx + 4] = (uint8_t)((value & 0x000000FF00000000) >> 32);
    mem->mem[idx + 5] = (uint8_t)((value & 0x0000FF0000000000) >> 40);
    mem->mem[idx + 6] = (uint8_t)((value & 0x00FF000000000000) >> 48);
    mem->mem[idx + 7] = (uint8_t)(value >> 56);
}

uint8_t mem_read8(memory_t* mem, uint64_t address) {
    if (!valid_access(address, 1)) {
        fprintf(stderr, "Invalid address: %llx\n", address);
        return 0;
    }
    
    size_t idx = address_to_index(address);

    return mem->mem[idx];
}

uint16_t mem_read16(memory_t* mem, uint64_t address) {
    if (!valid_access(address, 2)) {
        fprintf(stderr, "Invalid address: %llx\n", address);
        return 0;
    }

    size_t idx = address_to_index(address);

    return (uint16_t)((mem->mem[idx + 1] << 8) | mem->mem[idx]);
}

uint32_t mem_read32(memory_t* mem, uint64_t address) {
    if (!valid_access(address, 4)) {
        fprintf(stderr, "Invalid address: %llx\n", address);
        return 0;
    }

    size_t idx = address_to_index(address);

    return (uint32_t)((mem->mem[idx + 3] << 24) | (mem->mem[idx + 2] << 16) | (mem->mem[idx + 1] << 8) | (mem->mem[idx]));
}

uint64_t mem_read64(memory_t* mem, uint64_t address) {
    if (!valid_access(address, 8)) {
        fprintf(stderr, "Invalid address: %llx\n", address);
        return 0;
    }

    size_t idx = address_to_index(address);

    return
        ((uint64_t)mem->mem[idx + 7] << 56) |
        ((uint64_t)mem->mem[idx + 6] << 48) |
        ((uint64_t)mem->mem[idx + 5] << 40) |
        ((uint64_t)mem->mem[idx + 4] << 32) |
        ((uint64_t)mem->mem[idx + 3] << 24) |
        ((uint64_t)mem->mem[idx + 2] << 16) |
        ((uint64_t)mem->mem[idx + 1] << 8)  |
        ((uint64_t)mem->mem[idx]);
}


