#include <iostream>
#include "vm.h"
#include "instruction_types.h"

#include <stdio.h>

int main() {
    VM vm = VM();

    vm.ram.write32(Memory::MEM_BASE, 0x00B50533);
    
    RType ins = decodeR(0x00B50533);
}
