#include "vm.h"
#include "bus.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>

void vm_init(vm_t* vm) {
    vm->cpu = cpu_init();
    cpu_reset(&vm->cpu);
    vm->cpu.bus = &vm->bus;
    
    vm->ram = memory_init();

    vm->uart = uart_init();

    timer_reset(&vm->timer);
    
    vm->bus.ctx = vm;
    vm->bus.read8 = vm_bus_read8;
    vm->bus.read16 = vm_bus_read16;
    vm->bus.read32 = vm_bus_read32;
    vm->bus.read64 = vm_bus_read64;
    vm->bus.write8 = vm_bus_write8;
    vm->bus.write16 = vm_bus_write16;
    vm->bus.write32 = vm_bus_write32;
    vm->bus.write64 = vm_bus_write64;
}

void vm_free(vm_t* vm) {
    memory_free(&vm->ram);
}

int32_t vm_load_bin(vm_t* vm, const char* fpath) {
    errno = 0; 
    FILE* file = fopen(fpath, "rb");
    if (!file) {
        perror("fopen");
        return 1;
    }

    uint64_t offset = 0;
    char buffer[256];
    size_t bytes_read;
    
    do {
        bytes_read = fread(buffer, 1, 256, file);
        memcpy(vm->ram.mem + offset, buffer, bytes_read);
        offset += bytes_read;
    } while (bytes_read > 0);

    fclose(file);

    return 0;
}

void vm_tick(vm_t* vm) {
    timer_tick(&vm->timer);
    cpu_set_interrupt_pending(&vm->cpu, IRQ_TIMER, vm->timer.mtime >= vm->timer.mtimecmp);
}

