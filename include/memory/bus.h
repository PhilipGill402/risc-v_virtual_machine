#ifndef VM_INCLUDE_BUS_H_
#define VM_INCLUDE_BUS_H_

#include <stdint.h>

uint8_t vm_bus_read8(void* ctx, uint64_t addr);
uint16_t vm_bus_read16(void* ctx, uint64_t addr);
uint32_t vm_bus_read32(void* ctx, uint64_t addr);
uint64_t vm_bus_read64(void* ctx, uint64_t addr);

void vm_bus_write8(void* ctx, uint64_t addr, uint8_t value);
void vm_bus_write16(void* ctx, uint64_t addr, uint16_t value);
void vm_bus_write32(void* ctx, uint64_t addr, uint32_t value);
void vm_bus_write64(void* ctx, uint64_t addr, uint64_t value);

#endif
