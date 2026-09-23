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
    
    int32_t ret = vm_load_bin(&vm, "tests/program.bin");
    if (ret)
        exit(ret);

    vm.bus.write8(&vm, UART_BASE, 'A'); 

    vm_free(&vm);
}
