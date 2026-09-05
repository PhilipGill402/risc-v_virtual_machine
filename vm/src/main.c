#include "vm.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    vm_t vm = vm_init();
    
    int32_t ret = vm_load_bin(&vm, "tests/program.bin");
    if (ret)
        exit(ret);

    while (1) {
        cpu_step(&vm.cpu, &vm.ram);
    }
}
