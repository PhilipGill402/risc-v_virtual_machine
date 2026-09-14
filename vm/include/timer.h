#ifndef EMULATOR_INCLUDE_TIMER_H_
#define EMULATOR_INCLUDE_TIMER_H_

#include "cpu.h"

#define IRQ_TIMER 7

typedef struct timer {
    uint64_t mtime;
    uint64_t mtimecmp;
} timer_t;

void timer_tick(timer_t* timer, cpu_t* cpu);

#endif
