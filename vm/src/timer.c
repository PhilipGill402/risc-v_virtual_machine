#include "timer.h"
#include "csrs/csr_def.h"

void timer_tick(timer_t* timer, cpu_t* cpu) {
    timer->mtime += 1;

    if (timer->mtime >= timer->mtimecmp) 
        cpu_set_interrupt_pending(cpu, IRQ_TIMER, 1); 
    else
        cpu_set_interrupt_pending(cpu, IRQ_TIMER, 0);
}

