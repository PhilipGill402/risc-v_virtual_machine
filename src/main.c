#include "vm.h"
#include <stdio.h>

int main() {
    vm_t vm = vm_init();
    
    vm_load_bin(&vm, "tests/program.bin"); 
}
