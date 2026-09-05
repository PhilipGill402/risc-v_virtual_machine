#include "vm.h"
#include <stdio.h>

vm_t vm_init() {
    vm_t vm = { 0 };
    vm.cpu = cpu_init();
    vm.ram = memory_init();
    
    cpu_reset(&vm.cpu); 
    
    return vm;
}

