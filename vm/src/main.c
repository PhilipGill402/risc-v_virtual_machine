#include "vm.h"
#include "csrs/csr_def.h"
#include "csrs/csr.h"
#include <stdio.h>
#include <stdlib.h>

#include <assert.h>

#include "mmio.h"

int main() {
    vm_t vm = { 0 };
    vm_init(&vm);

    csr_load(&vm.cpu);
    
    int32_t ret = vm_load_bin(&vm, "tests/program.bin");
    if (ret)
        exit(ret);

    vm.timer.mtime = 10;
    vm.timer.mtimecmp = 5;

    // Make sure MTIP starts clear
    cpu_set_interrupt_pending(&vm.cpu, IRQ_TIMER, 0);

    vm_tick(&vm);

    assert(vm.timer.mtime == 11);

    // Check mip.MTIP
    uint64_t mip = vm.cpu.csrs[CSR_MIP];

    assert(mip & (1ULL << IRQ_TIMER));

    printf("test_timer_sets_mtip passed\n");

    vm_free(&vm);
}
