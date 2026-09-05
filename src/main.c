#include "vm.h"
#include <stdio.h>

int main() {
    vm_t vm = vm_init();
    
    mem_write32(&vm.ram, MEM_BASE, 0xFFB28313);
    cpu_step(&vm.cpu, &vm.ram);
   
    printf("%lld\n", (int64_t)cpu_read_reg(&vm.cpu, 6));
}
