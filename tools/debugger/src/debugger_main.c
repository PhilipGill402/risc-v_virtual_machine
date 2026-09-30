#include <stdio.h>
#include "cli.h"
#include "cpu/cpu.h"
#include "memory/mmio.h"

int main() {
    vm_t vm;
    vm_init(&vm);

    vm.cpu.priviledge = U_MODE;
    
    int8_t ret = cli_run(&vm); 

    vm_free(&vm);
    return ret;
}
