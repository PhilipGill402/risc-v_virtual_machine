#include "bus.h"
#include "mmio.h"
#include "memory.h"
#include "timer.h"
#include "uart.h"
#include "vm.h"
#include "log.h"

#include <stdlib.h>

static uint8_t addr_in_range(uint64_t address, uint64_t base, uint64_t size) {
    return address >= base && (address - base) < size;
}

uint8_t vm_bus_read8(void* ctx, uint64_t addr) {
    vm_t* vm = (vm_t*)ctx;

    if (addr_in_range(addr, RAM_BASE, RAM_SIZE))
        return mem_read8(&vm->ram, addr);

    if (addr_in_range(addr, UART_BASE, UART_SIZE))
        return uart_read8(&vm->uart, addr - UART_BASE);

    if (addr_in_range(addr, TIMER_BASE, TIMER_SIZE)) {
        log_error("timer 8 bit write not supported\n");
        return 0;
    }

    log_error("Invalid physical read at 0x%llx\n", addr);
    return 0;
}

uint16_t vm_bus_read16(void* ctx, uint64_t addr) {
    vm_t* vm = (vm_t*)ctx;

    if (addr_in_range(addr, RAM_BASE, RAM_SIZE))
        return mem_read16(&vm->ram, addr);

    if (addr_in_range(addr, UART_BASE, UART_SIZE)) {
        log_error("UART 16 bit reads currently aren't supported\n");
        return 0;
        //return uart_read16(&vm->uart, addr - UART_BASE);
    }

    if (addr_in_range(addr, TIMER_BASE, TIMER_SIZE)) {
        log_error("timer 16 bit write not supported\n");
        return 0;
    }

    log_error("Invalid physical read at 0x%llx\n", addr);
    return 0;
}

uint32_t vm_bus_read32(void* ctx, uint64_t addr) {
    vm_t* vm = (vm_t*)ctx;

    if (addr_in_range(addr, RAM_BASE, RAM_SIZE))
        return mem_read32(&vm->ram, addr);

    if (addr_in_range(addr, UART_BASE, UART_SIZE)) {
        log_error("UART 32 bit reads currently aren't supported\n");
        return 0;
        //return uart_read32(&vm->uart, addr - UART_BASE);
    }

    if (addr_in_range(addr, TIMER_BASE, TIMER_SIZE)) {
        return timer_read32(&vm->timer, addr - TIMER_BASE);
    }

    log_error("Invalid physical read at 0x%llx\n", addr);
    return 0;
}

uint64_t vm_bus_read64(void* ctx, uint64_t addr) {
    vm_t* vm = (vm_t*)ctx;

    if (addr_in_range(addr, RAM_BASE, RAM_SIZE))
        return mem_read64(&vm->ram, addr);
    
    if (addr_in_range(addr, UART_BASE, UART_SIZE)) {
        log_error("UART 64 bit reads currently aren't supported\n");
        return 0;
        //return uart_read64(&vm->uart, addr - UART_BASE);
    }

    if (addr_in_range(addr, TIMER_BASE, TIMER_SIZE))
        return timer_read64(&vm->timer, addr - TIMER_BASE);

    log_error("Invalid physical read at 0x%llx\n", addr);
    return 0;
}

void vm_bus_write8(void* ctx, uint64_t addr, uint8_t value) {
    vm_t* vm = (vm_t*)ctx;

    if (addr_in_range(addr, RAM_BASE, RAM_SIZE)) {
        mem_write8(&vm->ram, addr, value);
        return;
    }

    if (addr_in_range(addr, UART_BASE, UART_SIZE)) {
        uart_write8(&vm->uart, addr - UART_BASE, value);
        return;
    }

    if (addr_in_range(addr, TIMER_BASE, TIMER_SIZE)) {
        log_error("timer 8 bit read not supported\n");
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

    if (addr_in_range(addr, UART_BASE, UART_SIZE)) {
        log_error("UART 16 bit writes currently aren't supported\n");
        //uart_write16(&vm->uart, addr - UART_BASE, value);
        return;
    }

    if (addr_in_range(addr, TIMER_BASE, TIMER_SIZE)) {
        log_error("timer 16 bit read not supported\n");
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

    if (addr_in_range(addr, UART_BASE, UART_SIZE)) {
        log_error("UART 32 bit writes currently aren't supported\n");
        //uart_write32(&vm->uart, addr - UART_BASE, value);
        return;
    }

    if (addr_in_range(addr, TIMER_BASE, TIMER_SIZE)) {
        timer_write32(&vm->timer, addr - TIMER_BASE, value);
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
    
    if (addr_in_range(addr, UART_BASE, UART_SIZE)) {
        log_error("UART 64 bit writes currently aren't supported\n"); 
        //uart_write64(&vm->uart, addr - UART_BASE, value);
        return;
    }

    if (addr_in_range(addr, TIMER_BASE, TIMER_SIZE)) {
        timer_write64(&vm->timer, addr - TIMER_BASE, value);
        return;
    }

    printf("BAD WRITE pc=0x%016llx addr=0x%016llx\n", vm->cpu.pc, addr);

    for (int i = 0; i < 32; i++) {
        printf("x%-2d = 0x%016llx%s", i, cpu_read_reg(&vm->cpu, i), ((i + 1) % 4 == 0) ? "\n" : "  ");
    }

    exit(1);

    log_error("Invalid physical write at 0x%llx\n", addr);
    return;
}


