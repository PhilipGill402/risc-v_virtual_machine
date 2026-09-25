#include "vm.h"
#include "csrs/csr_def.h"
#include "csrs/csr.h"
#include <stdio.h>
#include <stdlib.h>
#include "mmio.h"

int main() {
    vm_t vm = { 0 };
    vm_init(&vm);

    csr_load(&vm.cpu);
    
    int32_t ret = vm_load_bin(&vm, "vm/fw_jump.bin", RAM_BASE);
    if (ret)
        exit(ret);
    
    ret = vm_load_bin(&vm, "vm/vm.dtb", RAM_BASE + 0x02000000);
    if (ret)
        exit(ret);

    ret = vm_load_bin(&vm, "vm/image.bin", 0x80200000);
    if (ret)
        exit(ret);
    
    vm.cpu.pc = 0x80000000;

    vm.cpu.regs[10] = 0;          // a0 = hart ID
    vm.cpu.regs[11] = 0x82000000; // a1 = DTB address
                                  
    vm.cpu.priviledge = M_MODE;
    cpu_t* cpu = &vm.cpu;
    
    while (1) {
        //printf("instruction = 0x%08x\n", mem_read32(&vm.ram, vm.cpu.pc));
        cpu_step(&vm.cpu, &vm.ram);
        vm_tick(&vm);
    }
    
    vm_free(&vm);
}
