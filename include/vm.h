#ifndef INCLUDE_VM_H_
#define INCLUDE_VM_H_

#include "cpu.h"
#include "memory.h"

typedef struct VM {
    cpu_t cpu;
    memory_t ram;
} vm_t;

vm_t vm_init();

#endif
