#include <iostream>
#include "cpu.h"


int main() {
    CPU riscv = CPU();

    riscv.regs.write(0, 10);
    riscv.regs.write(1, 64);
    
    std::cout << riscv.regs.read(0) << "\n";
    std::cout << riscv.regs.read(1) << "\n";
}
