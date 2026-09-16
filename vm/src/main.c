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

    cpu_t* cpu = &vm.cpu;
    memory_t* mem = &vm.ram;
    
    cpu->priviledge = U_MODE;
    cpu->csrs[CSR_MEDELEG] |= 1 << 8;
    cpu->csrs[CSR_STVEC] = MEM_BASE + 100;
    mem_write32(mem, MEM_BASE, 0x00000073);

    cpu_step(cpu, mem);

    printf("SEPC: %llx\n", cpu->csrs[CSR_SEPC]);
    printf("SCAUSE: %lld\n", cpu->csrs[CSR_SCAUSE]);
    printf("SPP: %d\n", (uint8_t)(cpu->csrs[CSR_MSTATUS] >> 8) & 0x1);
    printf("Priviledge: %d\n", cpu->priviledge);
    printf("PC: %llx\n", cpu->pc);
}
