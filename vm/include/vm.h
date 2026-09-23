#ifndef INCLUDE_VM_H_
#define INCLUDE_VM_H_

#include <stdint.h>
#include "cpu.h"
#include "memory.h"
#include "timer.h"

typedef struct VM {
    cpu_t cpu;
    bus_t bus;
    memory_t ram;
    timer_t timer;
} vm_t;

void vm_init(vm_t* vm);
void vm_free(vm_t* vm);
uint64_t bus_read64(vm_t* vm, uint64_t address);
void bus_write64(vm_t* vm, uint64_t address, uint64_t value);

int32_t vm_load_bin(vm_t* vm, const char* fpath);

#endif
