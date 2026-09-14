#include "vm.h"
#include "csrs/csr_def.h"
#include "csrs/csr.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    vm_t vm = vm_init();
    csr_load(&vm.cpu);

    vm.timer.mtimecmp = 10;
    
    int32_t ret = vm_load_bin(&vm, "tests/program.bin");
    if (ret)
        exit(ret);
    
    // testing interrupts
    for (uint8_t i = 0; i < 10; i++) {
        timer_tick(&vm.timer, &vm.cpu);
    }

    cpu_t* cpu = &vm.cpu;
    cpu->csrs[CSR_MIE] |= 1 << 7;
    cpu->csrs[CSR_MSTATUS] |= 1 << 3;

    cpu_step(&vm.cpu, &vm.ram);


    printf("MEPC: %x\n", cpu->csrs[CSR_MEPC]);
    printf("MCAUSE: %x\n", cpu->csrs[CSR_MCAUSE]);
    printf("MTVAL: %x\n", cpu->csrs[CSR_MTVAL]);
    
    uint64_t mstatus = cpu->csrs[CSR_MSTATUS];
    printf("mstatus.MIE: %d\n", (uint8_t)(mstatus >> 3) & 0x1);
    printf("mstatus.MIP: %d\n", (uint8_t)(mstatus >> 7) & 0x1);
    printf("mstatus.MPP: %d\n", (uint8_t)(mstatus >> 11) & 0x3);

    printf("Priviledge: %d\n", cpu->priviledge);
    printf("PC: %llx\n", cpu->pc);
}
