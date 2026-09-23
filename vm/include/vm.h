#ifndef INCLUDE_VM_H_
#define INCLUDE_VM_H_

#include <stdint.h>
#include "cpu.h"
#include "memory.h"
#include "timer.h"
#include "uart.h"

typedef struct VM {
    cpu_t cpu;
    bus_t bus;
    memory_t ram;
    timer_t timer;
    uart_t uart;
} vm_t;

void vm_init(vm_t* vm);
void vm_free(vm_t* vm);
void vm_tick(vm_t* vm);

int32_t vm_load_bin(vm_t* vm, const char* fpath);

#endif
