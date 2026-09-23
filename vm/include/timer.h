#ifndef EMULATOR_INCLUDE_TIMER_H_
#define EMULATOR_INCLUDE_TIMER_H_

#include <stdint.h>

#define IRQ_TIMER 7
#define TIMER_MTIMECMP 0x4000
#define TIMER_MTIME    0xBFF8

typedef struct timer {
    uint64_t mtime;
    uint64_t mtimecmp;
} timer_t;

void timer_reset(timer_t* timer);
void timer_tick(timer_t* timer);
uint64_t timer_read64(timer_t* timer, uint64_t offset);
void timer_write64(timer_t* timer, uint64_t offset, uint64_t value);

#endif
