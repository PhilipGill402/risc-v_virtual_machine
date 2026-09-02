#include <iostream>
#include "vm.h"

#include <stdio.h>

int main() {
    VM vm = VM();

    vm.ram.write32(Memory::MEM_BASE, 0xFFB28313);
    
    vm.cpu.step(vm.ram);

    printf("%lld\n", static_cast<int64_t>(vm.cpu.read_reg(6)));
}
