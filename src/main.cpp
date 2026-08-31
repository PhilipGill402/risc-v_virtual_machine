#include <iostream>
#include "vm.h"

#include <stdio.h>

int main() {
    VM vm = VM();

    vm.ram.write64(Memory::MEM_BASE, 5000000000);
    
    printf("%llu\n", vm.ram.read64(Memory::MEM_BASE));
}
