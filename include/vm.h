#ifndef INCLUDE_VM_H_
#define INCLUDE_VM_H_

#include "cpu.h"
#include "memory.h"

class VM {
public:
    CPU cpu;
    Memory ram;

    VM() = default;
};

#endif
