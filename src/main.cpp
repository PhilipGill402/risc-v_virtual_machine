#include <iostream>
#include "vm.h"

#include <stdio.h>

int main() {
    VM vm = VM();

    vm.ram.write8(Memory::MEM_BASE, 67);
    
    printf("%d\n", vm.ram.read8(Memory::MEM_BASE));
}
