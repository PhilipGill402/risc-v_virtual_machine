#ifndef INCLUDE_VM_H_
#define INCLUDE_VM_H_

#include <stdint.h>
#include "cpu.h"
#include "memory.h"
#include "timer.h"

typedef struct VM {
    cpu_t cpu;
    memory_t ram;
    timer_t timer;
} vm_t;

vm_t vm_init();
void vm_free(vm_t* vm);

int32_t vm_load_bin(vm_t* vm, const char* fpath);

#endif
