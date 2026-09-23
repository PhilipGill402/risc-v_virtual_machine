#include "bus.h"
#include "mmio.h"
#include "memory.h"
#include "timer.h"
#include "vm.h"
#include "log.h"

static uint8_t addr_in_range(uint64_t address, uint64_t base, uint64_t size) {
    return address >= base && (address - base) < size;
}

uint8_t vm_bus_read8(void* ctx, uint64_t addr) {
    vm_t* vm = (vm_t*)ctx;

    if (addr_in_range(addr, RAM_BASE, RAM_SIZE))
        return mem_read8(&vm->ram, addr);

    log_error("Invalid physical read at 0x%llx\n", addr);
    return 0;
}

uint16_t vm_bus_read16(void* ctx, uint64_t addr) {
    vm_t* vm = (vm_t*)ctx;

    if (addr_in_range(addr, RAM_BASE, RAM_SIZE))
        return mem_read16(&vm->ram, addr);

    log_error("Invalid physical read at 0x%llx\n", addr);
    return 0;
}

uint32_t vm_bus_read32(void* ctx, uint64_t addr) {
    vm_t* vm = (vm_t*)ctx;

    if (addr_in_range(addr, RAM_BASE, RAM_SIZE))
        return mem_read32(&vm->ram, addr);

    log_error("Invalid physical read at 0x%llx\n", addr);
    return 0;
}

uint64_t vm_bus_read64(void* ctx, uint64_t addr) {
    vm_t* vm = (vm_t*)ctx;

    if (addr_in_range(addr, RAM_BASE, RAM_SIZE))
        return mem_read64(&vm->ram, addr);

    log_error("Invalid physical read at 0x%llx\n", addr);
    return 0;
}

void vm_bus_write8(void* ctx, uint64_t addr, uint8_t value) {
    vm_t* vm = (vm_t*)ctx;

    if (addr_in_range(addr, RAM_BASE, RAM_SIZE)) {
        mem_write8(&vm->ram, addr, value);
        return;
    }

    log_error("Invalid physical write at 0x%llx\n", addr);
    return;
}

void vm_bus_write16(void* ctx, uint64_t addr, uint16_t value) {
    vm_t* vm = (vm_t*)ctx;

    if (addr_in_range(addr, RAM_BASE, RAM_SIZE)) {
        mem_write16(&vm->ram, addr, value);
        return;
    }

    log_error("Invalid physical write at 0x%llx\n", addr);
    return;
}

void vm_bus_write32(void* ctx, uint64_t addr, uint32_t value) {
    vm_t* vm = (vm_t*)ctx;

    if (addr_in_range(addr, RAM_BASE, RAM_SIZE)) {
        mem_write32(&vm->ram, addr, value);
        return;
    }

    log_error("Invalid physical write at 0x%llx\n", addr);
    return;
}

void vm_bus_write64(void* ctx, uint64_t addr, uint64_t value) {
    vm_t* vm = (vm_t*)ctx;

    if (addr_in_range(addr, RAM_BASE, RAM_SIZE)) {
        mem_write64(&vm->ram, addr, value);
        return;
    }

    log_error("Invalid physical write at 0x%llx\n", addr);
    return;
}


